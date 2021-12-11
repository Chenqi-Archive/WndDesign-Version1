#pragma once

#include "wnd_base.h"
#include "../render/canvas.h"
#include "../figure/figure.h"

BEGIN_NAMESPACE(WndDesign)

#pragma warning(disable : 4250)  // 'class1' : inherits 'class2::member' via dominance


class _Wnd_Impl;

class _WndBase_Impl : virtual public IWndBase, Uncopyable {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
private:
	Ref<_WndBase_Impl*> _parent;  // Parent window will be reset when canvas changed.
public:
	_WndBase_Impl(Alloc<WndStyle*> style, Ref<ObjectBase*> object);
	virtual ~_WndBase_Impl();

	Ref<IWndBase*> GetParent() override final { return _parent; }
	// Called when parent has changed or the window destroys.
	void DetachFromParent() { if (_parent != nullptr) { _parent->RemoveChild(this); } }
	void Destroy() override { delete this; }  // Only overrided by desktop.

private:
	Data _user_data;
	void SetUserData(Data data) override final { _user_data = data; }
	Data GetUserData() const override final { return _user_data; }


	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
private:
	Alloc<WndStyle*> _style;
public:
	const WndStyle& GetStyle() const { return *_style; }	// For parent window to parse the style.
	bool SetRegion(ValueTag width, ValueTag height, ValueTag left, ValueTag top) override final;
	void SetOpacity(uchar opacity) override final;
	void SetBackground(Color background) override final;


	/////////////////////////////////////////////////////////
	////                    Composite                    ////
	/////////////////////////////////////////////////////////
private:
	// The canvas that the window can draw on.
	Canvas _canvas;

	// The max_size and min size of the window(in pixels), set by parent window.
	// When parent window's size has changed, the size style will also change.
	// It can be used for windows to determine the proper size.
	//   (For example, for textbox to auto resize when text changes.)
	Size _size_min, _size_max;
public:
	void SetSizeStyle(Size size_min, Size size_max) { _size_min = size_min; _size_max = size_max; }
	const Size GetSizeMin() const { return _size_min; }
	const Size GetSizeMax() const { return _size_max; }

public:
	const bool IsEmpty() const { return _canvas.IsEmpty(); }
	const Ref<Layer*> GetLayer() const { return _canvas.GetLayer(); }
	const Size GetSize() const override final { return _canvas.GetSize(); }
	const Rect GetVisibleRegion() const { return _canvas.GetVisibleRegion(); }

	const Point ConvertToLayerPoint(Point point) const override final { 
		return _canvas.ConvertToLayerPoint(point); 
	}
	const Point ConvertToParentPoint(Point point) const override final { 
		return _canvas.ConvertToParentPoint(point); 
	}
	const Point ConvertToDesktopPoint(Point point) const override final {
		return _canvas.ConvertToDesktopPoint(point); // If myself is desktop, _canvas will return the point itself.
	}
	const Rect RegionOnParent() const { 
		return { ConvertToParentPoint(point_zero), _canvas.GetSize() }; 
	}
public:
	void RegionUpdated(Rect region) const { return _canvas.RegionUpdated(region); }
	void DrawFigureQueue(const FigureQueue& figure_queue, Rect bounding_region, 
						 Vector position_offset = vector_zero) const {
		return _canvas.DrawFigureQueue(figure_queue, bounding_region, position_offset);
	}

	// Called by parent window when added to a parent, reset its region, or when parent resizes.
	// If size has changed, Msg::Resize with new size as the parameter will be sent.
	// If the canvas's layer has changed, the child layers' target resources will be cleared.
	void SetCanvas(const Canvas& canvas);
private:
	// Called by the upper SetCanvas() if size has changed.
	// You are expected to recalculate the size and reset canvas for child windows.
	virtual void SizeChanged() { /* Do nothing, WndBase has no child window. */ }

public:
	// The canvas's visible region has changed, refresh the visible region for all child layers,
	//   and then recomposite.
	virtual void RefreshVisibleRegion(Rect visible_region) { return; }

	// Composite updated region the on the canvas.
	// Only called by its parent layer.
	virtual void Composite(Rect region_to_update) const;

private:
	// If is redrawing child layers, the layer updated message will be blocked to prevent composition.
	mutable bool _is_blocking_layer = false;
protected:
	void BlockLayerUpdate() const { _is_blocking_layer = true; }
	void AllowLayerUpdate() const { _is_blocking_layer = true; }
	bool IsLayerUpdateAllowed() const { return !_is_blocking_layer; }
public:
	// The region on the layer has updated, the window has to recomposite.
	// The composite operation may be masked or delayed.
	virtual void LayerUpdated(Ref<const Layer*> layer, Rect region) const { return; }


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
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
public:
	// Add the child window. WndBase should have no child window.
	bool AddChild(Ref<IWndBase*> child_wnd) override { return false; }
	// Remove the child window, but not destroy the window.
	bool RemoveChild(Ref<IWndBase*> child_wnd) override { return false; }


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////

	//// Message Handling ////
private:
	Ref<ObjectBase*> _object = nullptr;
	void SetObject(Ref<ObjectBase*> object) override final { _object = object; }

public:
	virtual bool DispatchMessage(Msg msg, Para para);

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


END_NAMESPACE(WndDesign)