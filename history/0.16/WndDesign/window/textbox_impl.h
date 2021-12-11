#pragma once

#include "textbox.h"
#include "wnd_base_impl.h"

#include "../render/direct2d/dwrite.h"


BEGIN_NAMESPACE(WndDesign)


class _TextBox_Impl : virtual public ITextBox, public _WndBase_Impl {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
public:
	_TextBox_Impl(Alloc<TextBoxStyle*> style, Ref<ObjectBase*> object);
	~_TextBox_Impl() override {}


	/////////////////////////////////////////////////////////
	////                    Composite                    ////
	/////////////////////////////////////////////////////////
public:
	void SizeChanged() override;
	void Composite(Rect region_to_update) const override;


	//////////////////////////////////////////////////////////
	////                       Text                       ////
	//////////////////////////////////////////////////////////
protected:
	wstring _text;
	TextLayout _layout;
	Rect _text_region;

private:
	const Rect CalculateRegionOnParent(Size parent_size) const override;

public:
	void SetText(const wstring& text) override;
	const wstring& GetText() const override { return _text; }
};


END_NAMESPACE(WndDesign)