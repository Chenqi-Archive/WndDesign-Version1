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
	// Keeps the copy of the normal size for auto resizing when text changes.
	// calculate actualsize -> parent window sets canvas -> size actually changed.
	WndStyle::SizeStyle::SizeTag _size_normal_copy;
	// Returns true if textbox may auto resize.
	bool MayAutoResize() { return _size_normal_copy.width.IsAuto() || _size_normal_copy.height.IsAuto(); }
	// Returns true if size is changed.
	bool AutoResize();

public:
	void SetText(const wstring& text) override;
	const wstring& GetText() const override { return _text; }
};


END_NAMESPACE(WndDesign)