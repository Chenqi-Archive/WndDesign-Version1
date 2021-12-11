#pragma once

#include "text_box.h"
#include "wnd_base_impl.h"

BEGIN_NAMESPACE(WndDesign)


class _TextBox_Impl : virtual public TextBox, virtual public _WndBase_Impl {
	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
public:
	void SetExtendedStyle(const Ref TextBoxStyle* style) override;


	//////////////////////////////////////////////////////////
	////                      Render                      ////
	//////////////////////////////////////////////////////////

public:
	void Render() const override;


	////////////////////////////////////////////////////////////
	////                        Text                        ////
	////////////////////////////////////////////////////////////
private:
	wstring text;

public:
	void SetText(const wstring& text) override;
	wstring GetText() const override;




};


END_NAMESPACE(WndDesign)