#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"

#include "render_style.h"

#include <list>

BEGIN_NAMESPACE(WndDesign)

class RenderObject;
class RenderTargetBase;

class RenderLayer{
private:
	Ref RenderObject* _parent;
	Rect _region;

	// The layer style used when compositing. 
	// If the layer is the bottom most layer, then the style is nullptr.
	Ref const LayerStyle* _style;

	// The base render target for sub windows.
	Alloc RenderTargetBase* _target_base;

	std::list<const Ref RenderObject*> _sub_objects;

public:
	void AddObject(const Ref RenderObject* object);
	void TargetUpdated(const Rect& rect);

};


END_NAMESPACE(WndDesign)