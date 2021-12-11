#pragma once

#include "../../common/core.h"
#include "../../common/feature.h"
#include "../../geometry/geometry.h"

#include <unordered_map>
#include <vector>
#include <list>


//////////////////////////////////////////////////////////
////        Device Dependent Resources Manager        ////
//////////////////////////////////////////////////////////


////  Device dependent resources.

// Render targets.
struct ID2D1RenderTarget;
struct ID2D1HwndRenderTarget;
struct ID2D1BitmapRenderTarget;

// Bitmap.
struct ID2D1Bitmap;

// Brushes.
struct ID2D1SolidColorBrush;
struct ID2D1BitmapBrush;


BEGIN_NAMESPACE(WndDesign)

using std::unordered_map;
using std::list;
using std::vector;

using Handle = Ref<void*>;


class ImageResource;


struct Target : Uncopyable{
private:
	Alloc<ID2D1RenderTarget*> _target;

public:
	Target(Size size, const Ref<ID2D1RenderTarget*> compatable_target);
	Target(Target&& right) noexcept :_target(right._target) { right._target = nullptr; }
	~Target();
	void Recreate(Size size, const Ref<ID2D1RenderTarget*> compatable_target);

	ID2D1RenderTarget& GetTarget() const;
	const Size GetSize() const;

private:
	friend class TargetResourceManager;
	// Used only for desktop to create a system target.
	// Should not be destroyed.
	Target():_target(nullptr) {}
	void SetTarget(Ref<ID2D1RenderTarget*> target) { _target = target; }
};

using SizeLong = long long;
inline SizeLong SizeToLong(Size size) {
	static_assert(sizeof(Size) == sizeof(SizeLong), "");
	union { Size size; SizeLong as_long; } convert;
	convert.size = size;
	return convert.as_long;
}


class TargetResourceManager;

class TargetAllocator : Uncopyable{
private:
	const Ref<TargetResourceManager*> _resource_manager;
	const Size _size;

	list<Target> _targets_in_use;
	vector<Target> _targets_free;

public:
	TargetAllocator(const Ref<TargetResourceManager*> resource_manager, Size size);
	~TargetAllocator();

	// Return a target in the free stack. (As a pointer to an element appended in the used list)
	// If no target available, create a new one.
	Alloc<Target*> UseTarget();
	void ReturnTarget(Alloc<Target*> target);
	void RecreateResources();
	void CleanFreeTargets();
};


// Each system window has a resource manager storing the device dependent resources,
//   like the render targets and brushes.
class TargetResourceManager {
private:
	friend class SystemWndLayer;

	Handle _hWnd;

	// The HWND render target for each window.
	Alloc<ID2D1HwndRenderTarget*> _system_target;
	Target _system_target_container;

	// Reusable brushes.
	Alloc<ID2D1SolidColorBrush*> _solid_color_brush;
	//Alloc<ID2D1BitmapBrush> _bitmap_brush;

	// All allocated bitmap targets.
	// Indexed by size.
	unordered_map<SizeLong, TargetAllocator> _target_allocator;

	void CreateResources();
	void DestroyResources();

	void Resize();

public:
	TargetResourceManager(Handle hWnd);
	~TargetResourceManager();

	Handle GetSysWnd() { return _hWnd; }

	const Ref<ID2D1RenderTarget*> GetCompatableTarget() const;

	Alloc<Target*> UseTarget(Size size);
	void ReturnTarget(Alloc<Target*> target);

	ID2D1SolidColorBrush& UseSolidColorBrush() { return *_solid_color_brush; }
	//Ref<ID2D1BitmapBrush* UseBitmapBrush() const { return _bitmap_brush; }

	// When target resources are invalid, recreate all resources.
	void RecreateResources();
	// Set a timer to clean unused targets automatically.
	void CleanUnusedTargets();
};




END_NAMESPACE(WndDesign)