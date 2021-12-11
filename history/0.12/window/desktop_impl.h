#pragma once

#include "desktop.h"

#include "../render/render_target.h"
#include "../render/layer.h"

#include <list>

BEGIN_NAMESPACE(WndDesign)


using std::list;

class _Desktop_Impl : Desktop {
	// Disgarded functions.
private: 
	void SetStyle(const WndStyle& style) override {}
	void SetHandler(Handler handler) override {}
	Point ConvertToParentPoint(Point point) override { return point_zero; }
	
	//////////////////////////////////////////////////////////
	////                      Render                      ////
	//////////////////////////////////////////////////////////
private:


public:
	void Render();


	///////////////////////////////////////////////////////////
	////                   Child windows                   ////
	///////////////////////////////////////////////////////////
private:
	list<SystemWndLayer> _layers;

public:
	bool AddChild(Ref WndBase* child_wnd) override;
	bool RemoveChild(Ref WndBase* child_wnd) override;


	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
private:

public:
	// Enter the system message loop.
	void EnterMessageLoop();

	// Add a new dialog window and block the message input to parent window.
	void AddModalDialogBox(const Ref WndBase* dialog, const Ref WndBase* parent) override;
};


extern _Desktop_Impl* _desktop;


END_NAMESPACE(WndDesign)