#pragma once

#include "wnd_base.h"
#include "../render/canvas.h"
#include "../render/layer.h"
#include "../figure/figure.h"

BEGIN_NAMESPACE(WndDesign)

class _Wnd_Impl;

#pragma warning(disable : 4250)  // 'class1' : inherits 'class2::member' via dominance

class _WndBase_Impl : virtual public WndBase {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
private:
	Ref _WndBase_Impl* _parent;
public:
	_WndBase_Impl(Ref _WndBase_Impl* parent, const WndStyle& style, Handler handler);
	~_WndBase_Impl() override;

	void Destroy() override { delete this; }

private:
	Data _user_data;
	void SetUserData(Data data) override;
	Data GetUserData() const override;


	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
protected:
	WndStyle _style;
public:
	const WndStyle& GetStyle() const { return _style; }	// For parent window to parse the style.

	void SetSize(LengthTag width, LengthTag height) override;
	void SetPosition(LengthTag left, LengthTag top) override;
	void SetOpacity(uchar opacity) override;
	void SetBackground(Color32 background) override;
	Point ConvertToParentPoint(Point point) override;


	/////////////////////////////////////////////////////////
	////                    Composite                    ////
	/////////////////////////////////////////////////////////
protected:
	Canvas _canvas;

	// Like the "_is_redrawing_child" for layers.
	// If false, the parent window will recomposite immediately after a layer has updated.
	bool _is_redrawing_layer;

public:
	// Only overrided by desktop, for desktop, the visible region is the whole region.
	virtual Rect GetVisibleRegion() const {	
		return _canvas.GetVisibleRegion();
	}

	const Canvas& GetCanvas() const { return _canvas; }

	// Set the canvas, and recalculate the canvas for child windows.
	virtual void SetCanvas(const Canvas& canvas) {
		_canvas = canvas;
		Composite();
	}

	// The canvas's visible region has changed, refresh the visible region for all child layers,
	//   and then recomposite.
	virtual void RefreshVisibleRegion(){
		Composite();
	}

	// Composite updated region the on the canvas. 
	virtual void Composite(Rect region_to_update = region_infinite, const Ref Layer* layer = nullptr) const;

	// The region on the layer has updated, the window has to recomposite.
	// The composite operation may be masked or delayed.
	virtual void LayerUpdated(const Ref Layer* layer, Rect region);


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
protected:
	Handler _handler;
	void SetHandler(Handler handler) override { 
		_handler = handler; 
	}

	Ref _WndBase_Impl* _focus_wnd;  // Off: nullptr, On: this, Child: &Child.

public:
	virtual bool DispatchMessage(Msg msg, void* para) {
		// No child window, directly call the user's handler.
		if (_handler != nullptr) {
			return _handler(this, msg, para);
		}
		return false;
	}
};


END_NAMESPACE(WndDesign)