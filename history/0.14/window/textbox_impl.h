#pragma once

#include "textbox.h"
#include "wnd_base_impl.h"

BEGIN_NAMESPACE(WndDesign)


class _TextBox_Impl : virtual public TextBox, public _WndBase_Impl {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
public:
	_TextBox_Impl(Alloc TextBoxStyle* style, const wstring& text, Handler handler);
	~_TextBox_Impl() override {}


	/////////////////////////////////////////////////////////
	////                    Composite                    ////
	/////////////////////////////////////////////////////////
public:
	void Composite(Rect region_to_update) const override;


	////////////////////////////////////////////////////////////
	////                        Text                        ////
	////////////////////////////////////////////////////////////
protected:
	wstring _text;

	#pragma message("Save TextLayout here.")

public:
	void SetText(const wstring& text) override;
	const wstring& GetText() const override;
};


END_NAMESPACE(WndDesign)