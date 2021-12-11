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
public:
	Ref<IWndBase*> GetParent() override final { return _parent; }
private:
	friend class ChildWndContainer;
	// Called by parent window when adding child.
	void SetParent(Ref<_WndBase_Impl*> parent) { DetachFromParent(); _parent = parent; }
	// Called by current parent window when removing child.
	void ClearParent() { _parent = nullptr; }
private:
	// Called by myself when parent has changed or the window destroys.
	void DetachFromParent() { if (_parent != nullptr) { _parent->RemoveChild(this); } }


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
protected:
	// Child window container is a wrapper class for child windows to autometically set parent.
	// You can use any data structure to keep ChildWndContainer.
	class ChildWndContainer : WndDesign::Uncopyable {
	private:
		Ref<_WndBase_Impl*> parent;
		Ref<_WndBase_Impl*> wnd;
	public:
		ChildWndContainer(Ref<_WndBase_Impl*> child, Ref<_WndBase_Impl*> parent) : wnd(child), parent(parent) {
			wnd->SetParent(parent); 
		}
		~ChildWndContainer() {
			wnd->ClearParent();
			// Clear the focus and capture window.
			if (parent->_capture_wnd == wnd) { parent->_capture_wnd->LoseCapture(); parent->_capture_wnd = nullptr; }
			if (parent->_focus_wnd == wnd) {}
			if (parent->_last_child == wnd) { parent->_last_child = nullptr; }
		}
		operator Ref<_WndBase_Impl*>() const { return wnd; }
		Ref<_WndBase_Impl*> operator->() const { return wnd; }
	};

public:
	// Add the child window. WndBase should have no child window.
	bool AddChild(Ref<IWndBase*> child_wnd) override { return false; }
	// Remove the child window, but not destroy the window.
	bool RemoveChild(Ref<IWndBase*> child_wnd) override { return false; }
	// The child window's size has changed, update the size.
	bool UpdateChildRegion(Ref<IWndBase*> child_wnd) override { return false; }
	// Reset the child window's render style after the render style has changed.
	void UpdateChildRenderStyle(Ref<IWndBase*> child_wnd) override { return; }


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
	// You are expected to recalculate the size and reset canvas for child windows.
	virtual void SizeChanged() { return; } 

public:
	// The canvas's visible region has changed, refresh the visible region for all child layers,
	//   and then recomposite.
	virtual void RefreshVisibleRegion(Rect visible_region) { return; }

	// Composite updated region the on the canvas.
	// Only called by its parent layer.
	virtual void Composite(Rect region_to_update) const;

private:
	bool _is_composition_blocked = false;
public:
	void BlockComposition() override final {
		_is_composition_blocked = true;	
	}
	void UpdateRegion(Rect region) override final {
		_is_composition_blocked = false;
		RegionUpdated(region);
	}

protected:
	// For derived window to update a region and draw figure queue.
	void RegionUpdated(Rect region) const { 
		if (_is_composition_blocked) { return; }
		return _canvas.RegionUpdated(region); 
	}
	void DrawFigureQueue(FigureQueue& figure_queue, Rect bounding_region, 
						 Vector position_offset = vector_zero) const {
		if (_object != nullptr) { _object->AppendFigure(figure_queue); }
		_canvas.DrawFigureQueue(figure_queue, bounding_region, position_offset);
	}
public:
	// Called by base layer of Wnd to redirect figure queue.
	void RedirectLayerFigureQueue(const FigureQueue& figure_queue, Rect bounding_region,
								  Vector position_offset = vector_zero) const {
		_canvas.DrawFigureQueue(figure_queue, bounding_region, position_offset);
	}

private:
	// If is redrawing child layers, the layer updated message will be blocked to prevent composition.
	mutable bool _is_blocking_layer = false;
protected:
	void BlockLayerUpdate() const { _is_blocking_layer = true; }
	void AllowLayerUpdate() const { _is_blocking_layer = false; }
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


END_NAMESPACE(WndDesign)