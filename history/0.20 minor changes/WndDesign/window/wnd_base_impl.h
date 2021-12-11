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
	~_WndBase_Impl();

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
	}
	// Called by old parent window when myself was removd. Do NOT call DetachFromParent().
	void ClearParent() {
		if (_parent != nullptr) {
			_parent = nullptr;
		}
	}
private:
	// Called by myself when parent has changed or the window destroys.
	void DetachFromParent() { 
		if (_parent != nullptr) { 
			_parent->RemoveChild(this); 
			_parent = nullptr;
		} 
	}


	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
private:
	Alloc<WndStyle*> _style;
public:
	WndStyle& GetStyle() const override { return *_style; }	// For parent window to parse the style.

	bool RegionStyleUpdated() override {
		if (_parent != nullptr) { return _parent->UpdateChildRegion(this); }
		return false;
	}
	void RenderStyleUpdated() override {
		if (_parent != nullptr) { _parent->UpdateChildRenderStyle(this); }
	}

protected:
	// If the window's content has updated, it may auto resize. 
	// Return true if the window has actually resized (and the the window has already redrawn by parent).
	bool ContentUpdated() {
		if (GetStyle().auto_resize._MayAutoResize() && _parent != nullptr) {
			// If auto resizes, inform parent to recalculate region.
			bool size_changed = _parent->UpdateChildRegion(this);
			if (size_changed) { return true; }
		}
		return false;
	}

public:
	// Callback by parent window when adding child or child region has changed.
	// The window may auto resize depending panent size or its contents. (WndBase never auto resizes.)
	const Rect CalculateRegionOnParent(Size parent_size);

private:
	// Both auto resize.
	virtual Size AdjustSizeToContent(Size min_size, Size max_size) { return size_min; }
	// Only height auto resizes.
	virtual uint AdjustHeightToContent(uint min_height, uint max_height, uint width) { return 0; }
	// Only width auto resizes. (seldom used)
	virtual uint AdjustWidthToContent(uint min_width, uint max_width, uint height) { return 0; }


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
	virtual const Rect GetVisibleRegion() const { return _canvas.GetVisibleRegion(); }  // Only overrided by desktop.

	const Point ConvertToLayerPoint(Point point) const override final { return _canvas.ConvertToLayerPoint(point); }
	const Point ConvertToParentPoint(Point point) const override final { return _canvas.ConvertToParentPoint(point); }
	const Point ConvertToDesktopPoint(Point point) const override final { return _canvas.ConvertToDesktopPoint(point); }
	const Rect RegionOnLayer() const { return Rect(ConvertToLayerPoint(point_zero), _canvas.GetSize()); }
	const Rect RegionOnParent() const { return Rect(ConvertToParentPoint(point_zero), _canvas.GetSize()); }

public:
	// Called by parent window when added to a parent, reset its region, or when parent resizes.
	// If size has changed, Msg::Resize with new size as the parameter will be sent.
	// If the canvas's layer has changed, the child layers' target resources will be cleared.
	void SetCanvas(const Canvas& canvas, Rect visible_region);
private:
	// Called by the upper SetCanvas() if size has changed.
	// You are expected to recalculate layer regions and reset canvas for child windows.
	virtual void SizeChanged() { return; } 

public:
	// Called by parent layer when its cached region has changed.
	// Refresh the visible region for all child layers.
	// Also called when canvas is reset.
	virtual void RefreshVisibleRegion(Rect visible_region) { return; }

	// Composite updated region the on the canvas.
	// Only called by its parent layer.
	void Composite(Rect region_to_update) {
		FigureQueue figure_queue;
		AppendFigure(figure_queue, region_to_update);
		if (_object != nullptr) { _object->AppendFigure(figure_queue, region_to_update); }
		if (figure_queue.empty()) { return; }
		_canvas.DrawFigureQueue(figure_queue, region_to_update, vector_zero);
	}

	virtual void AppendFigure(FigureQueue& figure_queue, Rect region_to_update) const;


	// Used for combining multiple update operations to a single operation.
	// Also the same for layer.
private:
	class DelayedRegionUpdate {
	public:
		_WndBase_Impl* parent;
		bool is_parent_valid;
		DelayedRegionUpdate(_WndBase_Impl* parent) :parent(parent), is_parent_valid(true) { 
			if (parent->_delayed_update != nullptr) { 
				is_parent_valid = false; return;  // There's already one working.
			}
			parent->_delayed_update = this; 
			parent->BeginRegionUpdate(); 
		}
		~DelayedRegionUpdate() {
			// This trick allows your window destroy itself when handling messages.
			if (!is_parent_valid) { return; }  // The parent window has destroyed!
			parent->_delayed_update = nullptr;
			parent->EndRegionUpdate();
		}
	}* _delayed_update = nullptr;

	bool _is_composition_blocked = false;
	mutable Rect _region_to_update = region_empty;
	void BeginRegionUpdate() {
		_is_composition_blocked = true; 
		BeginLayerRegionUpdate();
	}
	void EndRegionUpdate() { 
		EndLayerRegionUpdate();
		_is_composition_blocked = false;
		UpdateRegion(_region_to_update);
		_region_to_update = region_empty;
	}
	virtual void BeginLayerRegionUpdate() { return; }
	virtual void EndLayerRegionUpdate() { return; }
protected:
	// For derived window to update a region and draw figure queue.
	void UpdateRegion(Rect region) const override{
		if (_is_composition_blocked) { 
			_region_to_update = _region_to_update.Union(region);
		} else {
			if (!region.IsEmpty()) { _canvas.RegionUpdated(region); }
		}
	}
public:
	// Called by base layer of Wnd to redirect figure queue.
	void RedirectLayerFigureQueue(const FigureQueue& figure_queue, Rect bounding_region, Vector position_offset) const {
		_canvas.DrawFigureQueue(figure_queue, bounding_region, position_offset);
	}

	// The region on the layer has updated, the window has to recomposite.
	// Layer may directly call parent window's RegionUpdated(), but the function is still kept here 
	//   because parent window can choose not to composite when the region is overlapped by top layers,
	//   though it has not implemented by now, which may require complex polygon calculations.
	/*virtual*/ void LayerUpdated(const Layer& layer, Rect region_on_layer) const {
		region_on_layer.point = layer.ConvertToParentPoint(region_on_layer.point);  // Now region is actually on parent.
		UpdateRegion(region_on_layer);  // It will return immediately if composition is blocked.
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
	////                     Scrolling                     ////
	///////////////////////////////////////////////////////////
public:
	const Point GetScrollPosition() const override { return point_zero; }
	void SetScrollPosition(Point point) override { return; }
	const Rect GetEntireRegion() const override { return Rect(point_zero, GetSize()); }


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////

	//// Mouse Tracking ////
protected:
	mutable Ref<_WndBase_Impl*> _last_child;  // Last child that receives mouse msg.


	//// Mouse Capture ////
protected:
	Ref<_WndBase_Impl*> _capture_wnd; // child, this or nullptr. 
									  // (capture window can be myself so that the mouse message will not be sent to child windows.)
public:
	void SetCapture() override final {
		if (_capture_wnd != nullptr && _capture_wnd != this) { _capture_wnd->LoseCapture(); }
		_capture_wnd = this;
		if (_parent != nullptr) { _parent->SetChildCapture(this); }
	}
	void ReleaseCapture() override final {
		if (_parent != nullptr) { _parent->ReleaseChildCapture(); }
	}
private:
	virtual void SetChildCapture(Ref<_WndBase_Impl*> child);	// Only overrided by desktop.
	virtual void ReleaseChildCapture(/*Ref<_WndBase_Impl*> child*/);  // Only overrided by desktop.
public:
	void LoseCapture() {
		if (_capture_wnd != nullptr) { 
			if (_capture_wnd != this) { _capture_wnd->LoseCapture(); }
			_capture_wnd = nullptr; 
		}
		DispatchMessage(Msg::LoseCapture, nullptr);
	}


	//// Input focus ////  (similar to mouse capture)
protected:
	Ref<_WndBase_Impl*> _focus_wnd;	// child, this or nullptr.
public:
	void SetFocus() override final {
		if (_focus_wnd != nullptr && _focus_wnd != this) { _focus_wnd->LoseFocus(); }
		_focus_wnd = this;
		if (_parent != nullptr) { _parent->SetChildFocus(this); }
	}
	void ReleaseFocus() override final {if (_parent != nullptr) { _parent->ReleaseChildFocus(); } }
private:
	virtual void SetChildFocus(Ref<_WndBase_Impl*> child);
	virtual void ReleaseChildFocus(/*Ref<_WndBase_Impl*> child*/);
public:
	void LoseFocus() {
		if (_focus_wnd != nullptr) { 
			if (_focus_wnd != this) { _focus_wnd->LoseFocus(); }
			_focus_wnd = nullptr; 
		}
		DispatchMessage(Msg::LoseFocus, nullptr);
	}


	//// Message Handling ////
private:
	Ref<ObjectBase*> _object = nullptr;
	bool _block_child_msg = false;
	void BlockChildMsg() override final { _block_child_msg = true; }

public:
	bool HandleMessage(Msg msg, Para para) const {
		if (_object != nullptr) { return _object->Handler(msg, para); }
		return true;  // The message is handled.
	}

	void DispatchMessage(Msg msg, Para para);


public:
	// Test if the window will handle the message at the point.
	// Returns true to accept the message and the real message will be sent later, returns false to deny handling the message.
	// Todo: It should be a virtual function. For Wnd, the background might be transparent but the layer has overlapped on it.
	bool MouseHitTest(Point point) const;

private:
	// Hit-test the child window to receive the message.
	virtual Ref<_WndBase_Impl*> HitTestChild(Point point) const { return nullptr; }
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
	ChildContainer& operator=(ChildContainer&& right) noexcept { ChildContainer tmp(std::move(right)); Swap(tmp); return *this; }
	~ChildContainer() {
		if (child == nullptr) { return; }
		_WndBase_Impl* parent = child->_parent;
		// Now child must have a perent. Clear the focus and capture window for its parent. 
		if (parent->_capture_wnd == child) { /*child->LoseCapture(); Dangerous!*/ parent->_capture_wnd = nullptr; }
		if (parent->_focus_wnd == child) { /*child->LoseFocus(); Virtual function call at destruction!*/ parent->_focus_wnd = nullptr; }
		if (parent->_last_child == child) { parent->_last_child = nullptr; }
		child->ClearParent();
	}
	operator Ref<_WndBase_Impl*>() const { return child; }
	Ref<_WndBase_Impl*> operator->() const { return child; }

	// The copy contructor is only used for copying empty containers. 
	// I'd like to make ChildContainer Uncopyable, but STL containers does not support emplacing multiple elements. 
	ChildContainer(const ChildContainer& other) : child(nullptr) { if (other.child != nullptr) { Error(); } }
	ChildContainer& operator=(const ChildContainer& other) { child = nullptr; if (other.child != nullptr) { Error(); } return *this; }
};


END_NAMESPACE(WndDesign)


BEGIN_NAMESPACE(std)
// swap specialization for ChildContainer.
template<>
inline void swap<WndDesign::ChildContainer>(WndDesign::ChildContainer& left, WndDesign::ChildContainer& right) noexcept {
	left.Swap(right);
}
END_NAMESPACE(std)