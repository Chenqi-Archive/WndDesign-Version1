#include "direct2d.h"
#include "../../common/exception.h"

#undef Alloc
#include <d2d1.h>

#pragma comment(lib, "d2d1.lib")


BEGIN_NAMESPACE(WndDesign)

inline D2D1_SIZE_F SizeToSIZE(Size size) {
	return D2D1::SizeF(static_cast<FLOAT>(size.width), static_cast<FLOAT>(size.height));
}

inline Size SIZEToSize(D2D1_SIZE_F size) {
	return { static_cast<uint>(size.width), static_cast<uint>(size.height) };
}


HRESULT hr;
inline void _AbortWhenFailed(HRESULT hr) { if (FAILED(hr)) { std::abort(); } }
#define AbortWhenFailed(hr) _AbortWhenFailed(hr)

template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease) {
	if (*ppInterfaceToRelease != NULL) {
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = NULL;
	}
}

class DeviceIndepencentResources {
public:
	static Ref ID2D1Factory* factory() {
		static class Factory {
		public:
			ID2D1Factory* factory;
			Factory() { hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory); AbortWhenFailed(hr); }
			~Factory() { factory->Release(); }
		} factory;
		return factory.factory;
	}
};

Target::Target(Size size, const Ref ID2D1RenderTarget* compatable_target) {
	Ref ID2D1RenderTarget* the_compatable_target = const_cast<Ref ID2D1RenderTarget*>(compatable_target);
	the_compatable_target->CreateCompatibleRenderTarget(SizeToSIZE(size), &_target);
}

void Target::Recreate(Size size, const Ref ID2D1RenderTarget* compatable_target) {
	SafeRelease(&_target);
	Ref ID2D1RenderTarget* the_compatable_target = const_cast<Ref ID2D1RenderTarget*>(compatable_target);
	the_compatable_target->CreateCompatibleRenderTarget(SizeToSIZE(size), &_target);
}

void Target::Destroy() {
	SafeRelease(&_target);
}

const Size Target::GetSize() const {
	return SIZEToSize(_target->GetSize());
}

Ref ID2D1RenderTarget* Target::GetTarget() const { 
	return _target; 
}


// Return a target in the free stack. (As a pointer to an element appended in the used list)
// If no target available, create a new one.

TargetAllocator::TargetAllocator(const Ref TargetResourceManager* resource_manager, Size size) :
	_resource_manager(resource_manager), _size(size) {
}

TargetAllocator::~TargetAllocator() {
	if (!_targets_in_use.empty()) {
		BreakPoint();
		std::abort(); // Never reaches here.
	}
	// Destroy all free targets.
	while (!_targets_free.empty()) {
		_targets_free.top().Destroy(); _targets_free.pop();
	}
}

Ref Target* TargetAllocator::UseTarget() {
	if (_targets_free.empty()) {
		_targets_in_use.push_back(Target(_size, _resource_manager->GetCompatableTarget()));
	} else {
		_targets_in_use.push_back(_targets_free.top()); _targets_free.pop();
	}
	return &_targets_in_use.back();
}

void TargetAllocator::ReturnTarget(Ref Target* target) {
	for (auto it = _targets_in_use.begin(); it != _targets_in_use.end(); ++it) {
		if (&*it == target) {
			_targets_free.push(*target);
			_targets_in_use.erase(it);
			return;
		}
	}
}

void TargetAllocator::RecreateResources() {
	// Recreate all targets in use.
	for (auto& target : _targets_in_use) {
		target.Recreate(_size, _resource_manager->GetCompatableTarget());
	}
	// Destroy all free targets.
	while (!_targets_free.empty()) {
		_targets_free.top().Destroy(); _targets_free.pop();
	}
}


TargetResourceManager::TargetResourceManager():
	_hWnd(NULL), 
	_system_target(nullptr), 
	_system_target_container(),
	_solid_color_brush(nullptr) {
}

TargetResourceManager::~TargetResourceManager() {
	DestroyResources();
}

void TargetResourceManager::CreateResources() {
	HWND hWnd = static_cast<HWND>(_hWnd);
	if (hWnd == NULL) { return; }

	RECT rc;
	GetClientRect(hWnd, &rc);
	hr = DeviceIndepencentResources::factory()->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
		&_system_target);
	AbortWhenFailed(hr);
	_system_target_container.SetTarget(reinterpret_cast<ID2D1BitmapRenderTarget*>(_system_target));

	hr = _system_target->CreateSolidColorBrush(D2D1::ColorF(0xFFFFFFFF), &_solid_color_brush);
	AbortWhenFailed(hr);

	// create bitmap brush ...
}

void TargetResourceManager::DestroyResources() {
	SafeRelease(&_system_target);
	_system_target_container.SetTarget(nullptr);

	SafeRelease(&_solid_color_brush);
}

void TargetResourceManager::Resize() {
	HWND hWnd = static_cast<HWND>(_hWnd);
	RECT rc;
	GetClientRect(hWnd, &rc);
	_system_target->Resize(D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top));
}

void TargetResourceManager::Init(HANDLE hWnd) {
	_hWnd = hWnd;
	CreateResources();
}

const Ref ID2D1RenderTarget* TargetResourceManager::GetCompatableTarget() const { 
	return _system_target; 
}

Ref Target* TargetResourceManager::UseTarget(Size size) {
	auto it = _target_allocator.find(SizeToLong(size));
	if (it == _target_allocator.end()) {
		it = _target_allocator.insert(std::make_pair(SizeToLong(size), TargetAllocator(this, size))).first;
	}
	return it->second.UseTarget();
}

void TargetResourceManager::ReturnTarget(Ref Target* target) {
	auto it = _target_allocator.find(SizeToLong(target->GetSize()));
	if (it == _target_allocator.end()) {
		BreakPoint();
		std::abort(); // Never reaches here.
	}
	it->second.ReturnTarget(target);
}

void TargetResourceManager::RecreateResources() {
	DestroyResources();
	CreateResources();
	for (auto& it : _target_allocator) {
		it.second.RecreateResources();
	}
}

void TargetResourceManager::CleanUnusedTargets() {

}





END_NAMESPACE(WndDesign)