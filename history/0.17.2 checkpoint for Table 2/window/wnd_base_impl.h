#pragma once

#include "wnd_base.h"
#include "../render/canvas.h"
#include "../figure/figure.h"

#include "../common/exception.h"

BEGIN_NAMESPACE(WndDesign)


#pragma warning(disable : 4250)  // 'class1' : inherits 'class2::member' via dominance

class _WndBase_Impl : virtual public IWndBase, Uncopyable {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
public:
	_WndBase_Impl(Alloc<WndStyle*> style, Ref<ObjectBase*> object);
	virtual ~_WndBase_Impl();
	void Destroy() override { delete this; }  // Only overrided by desktop.

private:
	Ref<_WndBase_Impl*> _parent;  // Parent window will be reset when canvas changed.
protected:
	Ref<_WndBase_Impl*> GetParent() { return _parent; }
public:
	Ref<IWndBase*> GetParent() const override final { return _parent; }
public:
	// Called by parent window when myself was added as child.
	void SetParent(Ref<_WndBase_Impl*> parent) { 
		DetachFromParent();
		_parent = parent;
		DispatchMessage(Msg::Attach, _parent);
	}
	// Called by old parent window when myself was removd. Do NOT call DetachFromParent().
	void ClearParent() {
		if (_parent != nullptr) {
			DispatchMessage(Msg::Detach, _parent);
			_parent = nullptr;
		}
	}
private:
	// Called by myself when parent has changed or the window destroys.
	void DetachFromParent() { 
		if (_parent != nullptr) { 
			_parent->RemoveChild(this); 
			DispatchMessage(Msg::Detach, _parent);
			_parent = nullptr;
		} 
	}


	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
private:
	Alloc<WndStyle*> _style;
public:
	const WndStyle& GetStyle() const { return *_style; }	// For parent window to parse the style.
	bool SetRegion(ValueTag width, ValueTag height, ValueTag left, ValueTag top) override;  // Overrided by textbox to erase auto_resize.
	void SetOpacity(uchar opacity) override final;
	void SetBackground(const Background& background) override final;

	// Callback by parent window when adding myself or my region has changed.
	// Overrided by textbox to auto fit with text.
	virtual const Rect CalculateRegionOnParent(Size parent_size) const;


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
private:
	friend class ChildContainer;
public:
	// Add the child window. WndBase should have no child window.
	bool AddChild(Ref<IWndBase*> child_wnd) override { return false; }
	// Remove the child window, but not destroy the window.
	bool RemoveChild(Ref<IWndBase*> child_wnd) override { return false; }

	// Reset the child window's region after the position or size has changed.
	// Returns true if the window's actual region has really changed.
	virtual bool UpdateChildRegion(Ref<_WndBase_Impl*> child_wnd) { return false; }
	// Reset the child window's render style after the render style has changed.
	virtual void UpdateChildRenderStyle(Ref<_WndBase_Impl*> child_wnd) { return; }


	/////////////////////////////////////////////////////////
	////                    Composite                    ////
	/////////////////////////////////////////////////////////
private:
	// The canvas that the window can draw on.
	Canvas _canvas;

public:
	const bool IsEmpty() const { return _canvas.IsEmpty(); }
	const Size GetSize() const override final { return _canvas.GetSize(); }
	const Ref<Layer*> GetLayer() const { return _canvas.GetLayer(); }
	const Rect GetVisibleRegion() const { return _canvas.GetVisibleRegion(); }

	const Point ConvertToLayerPoint(Point point) const override final { return _canvas.ConvertToLayerPoint(point); }
	const Point ConvertToParentPoint(Point point) const override final { return _canvas.ConvertToParentPoint(point); }
	const Point ConvertToDesktopPoint(Point point) const override final { return _canvas.ConvertToDesktopPoint(point); }
	const Rect RegionOnLayer() const { return Rect(ConvertToLayerPoint(point_zero), _canvas.GetSize()); }
	const Rect RegionOnParent() const { return Rect(ConvertToParentPoint(point_zero), _canvas.GetSize()); }

public:
	// Called by parent window when added to a parent, reset its region, or when parent resizes.
	// If size has changed, Msg::Resize with new size as the parameter will be sent.
	// If the canvas's layer has changed, the child layers' target resources will be cleared.
	void SetCanvas(const Canvas& canvas);
private:
	// Called by the upper SetCanvas() if size has changed.
	// You are expected to recalculate layer regions and reset canvas for child windows.
	virtual void SizeChanged() { return; } 

public:
	// Called by parent layer when its cached region has changed.
	// Refresh the visible region for all child layers.
	virtual void RefreshVisibleRegion(Rect visible_region) { return; }

	// Composite updated region the on the canvas.
	// Only called by its parent layer.
	virtual void Composite(Rect region_to_update) const;

private:
	bool _is_composition_blocked = false;
public:
	void BlockComposition() override final { _is_composition_blocked = true; }
	void AllowComposition() override final { _is_composition_blocked = false; }
	void UpdateRegion(Rect region) override final { AllowComposition(); RegionUpdated(region); }

protected:
	// For derived window to update a region and draw figure queue.
	void RegionUpdated(Rect region) const { 
		if (_is_composition_blocked) { return; }
		return _canvas.RegionUpdated(region); 
	}
	void DrawFigureQueue(FigureQueue& figure_queue, Rect bounding_region, 
						 Vector position_offset = vector_zero) const {
		if (_object != nullptr) { _object->AppendFigure(figure_queue); }
		if (figure_queue.IsEmpty()) { return; }
		_canvas.DrawFigureQueue(figure_queue, bounding_region, position_offset);
	}
public:
	// Called by base layer of Wnd to redirect figure queue.
	void RedirectLayerFigureQueue(const FigureQueue& figure_queue, Rect bounding_region,
								  Vector position_offset = vector_zero) const {
		_canvas.DrawFigureQueue(figure_queue, bounding_region, position_offset);
	}

	// The region on the layer has updated, the window has to recomposite.
	// Layer may directly call parent window's RegionUpdated(), but the function is still kept here 
	//   because parent window can choose not to composite when the region is overlapped by top layers,
	//   though it has not implemented by now, which may require complex polygon calculations.
	/*virtual*/ void LayerUpdated(const Layer& layer, Rect region_on_layer) const {
		region_on_layer.point = layer.ConvertToParentPoint(region_on_layer.point);  // Now region is actually on parent.
		RegionUpdated(region_on_layer);  // It will return immediately if composition is blocked.
	}


	// For direct2d rendering. 
public:
	// When the window was attached to a new parent, the resource manager should be reset for all layers.
	virtual void ResetLayerResourceManager(Ref<TargetResourceManager*> resource_manager) { return; }

	Ref<TargetResourceManager*> GetResourceManager() {
		const Ref<Layer*> parent_layer = GetLayer();
		if (parent_layer != nullptr) { return parent_layer->GetResourceManager(); }
		return nullptr;
	}


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////

	//// Message Handling ////
private:
	Ref<ObjectBase*> _object = nullptr;

public:
	virtual bool DispatchMessage(Msg msg, Para para) {
		if (_object != nullptr) { return _object->Handler(msg, para); }
		return false;
	}

	// Test if the window will handle the message at the point.
	// If return true, the message has been handled, else, the message will be handled by the parent window.
	virtual bool MouseHitTest(Point point);


	//// Mouse Tracking ////
protected:
	Ref<_WndBase_Impl*> _last_child;  // Last child that receives mouse msg.


	//// Mouse Capture ////
protected:
	Ref<_WndBase_Impl*> _capture_wnd;		// &child, myself or nullptr
public:
	void SetCapture() override final { SetCapture(this); }
	virtual void SetCapture(_WndBase_Impl* wnd);	// Only overrided by desktop.
	void ReleaseCapture() override;  // Only overrided by desktop.
	void LoseCapture();


	//// Input focus ////
protected:
	Ref<_WndBase_Impl*> _focus_wnd;		// &child or nullptr
public:
	void SetFocus() override final { SetFocus(this); }
	virtual void SetFocus(Ref<_WndBase_Impl*> wnd);	// Only overrided by desktop.
	void ReleaseFocus() override;  // Only overrided by desktop.
	void LoseFocus();
};


// Child window container is a wrapper class for child windows to autometically set or clear parent.
// It is useful when parent removes multiple child windows or when parent destructs to free all child windows.
// You can use any data structure to keep ChildWndContainer.
class ChildContainer /* : WndDesign::Uncopyable*/ {
private:
	Ref<_WndBase_Impl*> child;
public:
	ChildContainer() : child(nullptr) {}
	ChildContainer(_WndBase_Impl& parent, Ref<_WndBase_Impl*> child) : child(child) { child->SetParent(&parent); }
	ChildContainer(ChildContainer&& right) noexcept : child(right.child) { right.child = nullptr; }
	void Swap(ChildContainer& other) noexcept { std::swap(child, other.child); }
	void operator=(ChildContainer&& right) noexcept { ChildContainer tmp(std::move(right)); Swap(tmp); }
	~ChildContainer() {
		if (child == nullptr) { return; }
		_WndBase_Impl* parent = child->_parent;
		// Now child must have a perent. Clear the focus and capture window for its parent. 
		if (parent->_capture_wnd == child) { child->LoseCapture(); parent->_capture_wnd = nullptr; }
		if (parent->_focus_wnd == child) { child->LoseFocus(); parent->_focus_wnd = nullptr; }
		if (parent->_last_child == child) { parent->_last_child = nullptr; }
		child->ClearParent();
	}
	operator Ref<_WndBase_Impl*>() const { return child; }
	Ref<_WndBase_Impl*> operator->() const { return child; }

	// The copy contructor is only used for copying empty containers. 
	// I'd like to make ChildContainer Uncopyable, but STL containers does not support emplacing multiple elements. 
	ChildContainer(const ChildContainer& other) : child(nullptr) { if (other.child != nullptr) { Error(); } }
	void operator=(const ChildContainer& other) = delete;
};


END_NAMESPACE(WndDesign)


BEGIN_NAMESPACE(std)
// swap specialization for ChildContainer.
template<>
void swap<WndDesign::ChildContainer>(WndDesign::ChildContainer& left, WndDesign::ChildContainer& right) noexcept {
	left.Swap(right);
}
END_NAMESPACE(std)