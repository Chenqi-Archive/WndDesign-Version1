#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"
#include "../texture/texture.h"
#include "../style/textbox_style.h"

#include <queue>
#include <stack>

BEGIN_NAMESPACE(WndDesign)

using std::queue;
using std::stack;

// The render target that finally do the rendering work, as tiles on a layer,
//   based on direct 2d.

class Figure;
class Layer;


class TargetImpl;

class RenderTarget {
private:
	Alloc TargetImpl* _target_impl;
	Layer* _layer;

public:
	void DrawLine(Point start_point, Point end_point);
	void DrawRectangle(Rect region, Color32 border_color, uint border_width, Color32 fill_color);
	void DrawRoundedRectangle(Rect region, uint radius, Color32 border_color, uint border_width, Color32 fill_color);
	void DrawEllipse(Rect region, Color32 border_color, uint border_width, Color32 fill_color);
	void DrawText(Rect region, const Ref TextBoxStyle* text_box_style);

	//void DrawColor(Color32 color, Rect region = region_infinite);
	//void DrawLine(Point start, Point end, Color32 color, uchar width);
	//void DrawArc(Point center, uint radius, ushort degree_begin = 0, ushort degree_end = 360);
	//void DrawText(const Ref TextBoxStyle* textbox_style, Rect region = region_infinite);

};


void ReleaseTargetImpl(TargetImpl*& target_impl) {
	if (target_impl != nullptr) {
		reinterpret_cast<ID2D1RenderTarget*>(target_impl)->Release();
		target_impl = nullptr;
	}
}

class TargetImplPool {
private:
	stack<Alloc TargetImpl*> pool;

public:
	void Push(Alloc TargetImpl* target) {
		pool.push(target);
	}
	Alloc TargetImpl* Pop() {
		if (pool.empty()) { return nullptr; }
		Alloc TargetImpl* target = pool.top();
		pool.pop();
		return target;
	}
	void Clear() {
		while (!pool.empty()) {
			ReleaseTargetImpl(pool.top());
			pool.pop();
		}
	}
	~TargetImplPool() { Clear(); }
};


END_NAMESPACE(WndDesign)