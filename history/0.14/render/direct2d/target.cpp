#include "target.h"
#include "../../common/exception.h"

#undef Alloc
#include <d2d1.h>

BEGIN_NAMESPACE(WndDesign)

inline D2D1_SIZE_F SizeToSIZE(Size size) {
	return D2D1::SizeF(static_cast<FLOAT>(size.width), static_cast<FLOAT>(size.height));
}

inline Size SIZEToSize(D2D1_SIZE_F size) {
	return { static_cast<uint>(size.width), static_cast<uint>(size.height) };
}


template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease) {
	if (*ppInterfaceToRelease != NULL) {
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = NULL;
	}
}


extern HRESULT hr;
inline void _AbortWhenFailed(HRESULT hr) { if (FAILED(hr)) { std::abort(); } }
#define AbortWhenFailed(hr) _AbortWhenFailed(hr)


extern Ref ID2D1Factory* GetFactroy();


Target::Target(Size size, const Ref ID2D1RenderTarget* compatable_target) :
	_target(nullptr) {
	Recreate(size, compatable_target);
}

Target::~Target() {
	SafeRelease(&_target);
}

void Target::Recreate(Size size, const Ref ID2D1RenderTarget* compatable_target) {
	SafeRelease(&_target);
	const_cast<Ref ID2D1RenderTarget*>(compatable_target)->CreateCompatibleRenderTarget(
		SizeToSIZE(size),
		reinterpret_cast<ID2D1BitmapRenderTarget**>(&_target)
	);
}

const Size Target::GetSize() const {
	return SIZEToSize(_target->GetSize());
}

Ref ID2D1RenderTarget* Target::GetTarget() const { 
	return _target; 
}



TargetAllocator::TargetAllocator(const Ref TargetResourceManager* resource_manager, Size size) :
	_resource_manager(resource_manager), _size(size) {
}

TargetAllocator::~TargetAllocator() {
	if (!_targets_in_use.empty()) { Error(); }
	CleanFreeTargets();
}

Ref Target* TargetAllocator::UseTarget() {
	if (_targets_free.empty()) {
		_targets_in_use.emplace_back(_size, _resource_manager->GetCompatableTarget());
	} else {
		_targets_in_use.emplace_back(std::move(_targets_free.back())); _targets_free.pop_back();
	}
	return &_targets_in_use.back();
}

void TargetAllocator::ReturnTarget(Ref Target* target) {
	for (auto it = _targets_in_use.begin(); it != _targets_in_use.end(); ++it) {
		if (&*it == target) {
			_targets_free.push_back(std::move(*target));
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
	CleanFreeTargets();
}

void TargetAllocator::CleanFreeTargets() {
	_targets_free.clear();
}


TargetResourceManager::TargetResourceManager(HANDLE hWnd):
	_hWnd(hWnd), 
	_system_target(nullptr), 
	_system_target_container(),
	_solid_color_brush(nullptr) {
	CreateResources();
}

TargetResourceManager::~TargetResourceManager() {
	DestroyResources();
}

void TargetResourceManager::CreateResources() {
	HWND hWnd = static_cast<HWND>(_hWnd);
	if (hWnd == NULL) { return; }

	RECT rc;
	GetClientRect(hWnd, &rc);
	hr = GetFactroy()->CreateHwndRenderTarget(
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

const Ref ID2D1RenderTarget* TargetResourceManager::GetCompatableTarget() const { 
	return _system_target; 
}

Ref Target* TargetResourceManager::UseTarget(Size size) {
	auto it = _target_allocator.find(SizeToLong(size));
	if (it == _target_allocator.end()) {
		it = _target_allocator.emplace(std::piecewise_construct, 
									   std::make_tuple(SizeToLong(size)), 
									   std::make_tuple(this, size)).first;
	}
	return it->second.UseTarget();
}

void TargetResourceManager::ReturnTarget(Ref Target* target) {
	auto it = _target_allocator.find(SizeToLong(target->GetSize()));
	if (it == _target_allocator.end()) { Error(); }
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
	for (auto& it : _target_allocator) {
		it.second.CleanFreeTargets();
	}
}





END_NAMESPACE(WndDesign)