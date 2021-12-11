#pragma once

#include "../../common/core.h"
#include "../../geometry/geometry.h"

#include <unordered_map>
#include <stack>
#include <list>


//////////////////////////////////////////////////////////
////                  Direct2D Types                  ////
//////////////////////////////////////////////////////////

////  Device dependent resources.

// Render targets.
struct ID2D1RenderTarget;
struct ID2D1HwndRenderTarget;
struct ID2D1BitmapRenderTarget;

// Brushes.
struct ID2D1SolidColorBrush;
struct ID2D1BitmapBrush;

////  Device independent resources.

// DirectWrite
class DirectWrite {

};


//////////////////////////////////////////////////////////
////        Device Dependent Resources Manager        ////
//////////////////////////////////////////////////////////

BEGIN_NAMESPACE(WndDesign)

using std::unordered_map;
using std::stack;
using std::list;

using HANDLE = void*;


struct Target {

private:
	Alloc ID2D1BitmapRenderTarget* _target;
public:
	Alloc Target(Size size, const Ref ID2D1RenderTarget* compatable_target);
	void Recreate(Size size, const Ref ID2D1RenderTarget* compatable_target);
	void Destroy();
	const Size GetSize() const;
	Ref ID2D1RenderTarget* GetTarget() const;

private:
	friend class TargetResourceManager;
	// Used only for desktop to create a system target.
	// Should not be destroyed.
	Target():_target(nullptr) {}
	void SetTarget(Ref ID2D1BitmapRenderTarget* target) { _target = target; }
};

using SizeLong = long long;
inline SizeLong SizeToLong(Size size) {
	static_assert(sizeof(Size) == sizeof(SizeLong), "");
	union { Size size; SizeLong as_long; } convert;
	convert.size = size;
	return convert.as_long;
}


class TargetResourceManager;

class TargetAllocator {
private:
	const Ref TargetResourceManager* _resource_manager;
	const Size _size;

	list<Alloc Target> _targets_in_use;
	stack<Alloc Target> _targets_free;

public:
	TargetAllocator(const Ref TargetResourceManager* resource_manager, Size size);
	~TargetAllocator();

	// Return a target in the free stack. (As a pointer to an element appended in the used list)
	// If no target available, create a new one.
	Ref Target* UseTarget();

	void ReturnTarget(Ref Target* target);

	void RecreateResources();
};


// Each system window has a resource manager storing the device dependent resources,
//   like the render targets and brushes.
class TargetResourceManager {
private:
	friend class SystemWndLayer;

	HANDLE _hWnd;
	// The HWND render target for each window.
	Alloc ID2D1HwndRenderTarget* _system_target;
	Target _system_target_container;

	// Reusable brushes.
	Alloc ID2D1SolidColorBrush* _solid_color_brush;
	//Alloc ID2D1BitmapBrush* _bitmap_brush;

	// All allocated bitmap targets.
	// Indexed by size.
	unordered_map<SizeLong, TargetAllocator> _target_allocator;

	void CreateResources();
	void DestroyResources();

	void Resize();

public:
	TargetResourceManager();
	~TargetResourceManager();

	void Init(HANDLE hWnd);

	const Ref ID2D1RenderTarget* GetCompatableTarget() const;

	Ref Target* UseTarget(Size size);
	void ReturnTarget(Ref Target* target);

	Ref ID2D1SolidColorBrush* UseSolidColorBrush() const { return _solid_color_brush; }
	//Ref ID2D1BitmapBrush* UseBitmapBrush() const { return _bitmap_brush; }


	// When target resources are invalid, recreate all resources.
	void RecreateResources();

	// Set a timer to clean unused targets automatically.
	void CleanUnusedTargets();
};




END_NAMESPACE(WndDesign)