#pragma once

#include "wnd_base.h"
#include "../render/canvas.h"
#include "../render/layer.h"
#include "../figure/figure.h"

BEGIN_NAMESPACE(WndDesign)

class _Wnd_Impl;

class _WndBase_Impl : virtual public WndBase {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
private:
	Ref _WndBase_Impl* _parent;
public:
	_WndBase_Impl(_WndBase_Impl* parent, const WndStyle* style, Handler handler);
	~_WndBase_Impl() override;


	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
private:
	WndStyle _style;
public:
	void SetStyle(const WndStyle& style) override;
	// For parent window to parse the style.
	const WndStyle& GetStyle() const { return _style; }


	//////////////////////////////////////////////////////////
	////                      Render                      ////
	//////////////////////////////////////////////////////////
private:
	Canvas _canvas;
public:
	Rect GetVisibleRegion() const {	return _canvas.GetVisibleRegion();}
	const Canvas& GetCanvas() const { return _canvas; }
	// Set the canvas.
	virtual void SetCanvas(const Canvas& canvas);
	// The parent layer's visible region has changed, refresh the child layer's visible region.
	virtual void RefreshVisibleRegion();
	// Draw the content on the canvas.
	virtual void Draw() const {
		queue<FigureContainer> figure_queue;
		figure_queue.push(FigureContainer(new Rectangle(), { {}, _canvas.GetSize() }));

		_canvas.DrawFigureQueue(figure_queue);
	}

	virtual void LayerUpdated(const Ref Layer* layer, Rect region){
		// Do nothing
	}

	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
private:
	Handler _handler;
	void SetHandler(Handler handler) override { _handler = handler; }
	virtual void DispatchMessage(Msg msg, void* para);

	Ref _WndBase_Impl* _focus_wnd;  // Off: nullptr, On: this, Child: &Child.
};


END_NAMESPACE(WndDesign)