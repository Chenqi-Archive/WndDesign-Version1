#pragma once

#include "button.h"
#include "textbox_impl.h"

BEGIN_NAMESPACE(WndDesign)

class _Button_Impl : virtual public Button, public _TextBox_Impl {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
public:
	_Button_Impl(Alloc ButtonStyle* style, const wstring& text, Handler handler);
	~_Button_Impl() override;

	/////////////////////////////////////////////////////////
	////                    Composite                    ////
	/////////////////////////////////////////////////////////
public:
	void Composite(Rect region_to_update) const override;

	///////////////////////////////////////////////////////////
	////                      Message                      ////
	///////////////////////////////////////////////////////////
private:
	Color32 _current_background;
	enum class State { Normal, Hover, Down } _state ;
public:
	bool DispatchMessage(Msg msg, void* para) override;
};


END_NAMESPACE(WndDesign)