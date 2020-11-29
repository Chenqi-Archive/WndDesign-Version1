#include "textbox_impl.h"
#include "../style/textbox_style_helper.h"

#include "../system/timer.h"
#include "../system/cursor.h"
#include "../system/ime.h"


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API unique_ptr<ITextBox> ITextBox::Create(Ref<IWndBase*> parent, const TextBoxStyle& style, Ref<ObjectBase*> object) {
	unique_ptr<ITextBox> textbox = std::make_unique<_TextBox_Impl>(new TextBoxStyle(style), object);
	if (parent != nullptr) { parent->AddChild(textbox.get()); }
	return textbox;
}


_TextBox_Impl::_TextBox_Impl(Alloc<TextBoxStyle*> style, Ref<ObjectBase*> object) :
	_WndBase_Impl(style, object),
	_text(nullptr),
	_layout(GetTextBoxStyle()),
	_text_region(region_empty) {
}

void _TextBox_Impl::SizeChanged() {
	_text_region = CalculateTextRegion(GetTextBoxStyle(), GetSize());
	_layout.SetMaxSize(_text_region.size);
}

void _TextBox_Impl::AppendFigure(FigureQueue& figure_queue, Rect region_to_update) const {
	const TextBoxStyle& style = GetTextBoxStyle();
	_WndBase_Impl::AppendFigure(figure_queue, region_to_update);  // Draw border and background.
	figure_queue.append(new TextFigure(_layout), _text_region);
}


Size _TextBox_Impl::AdjustSizeToContent(Size min_size, Size max_size) {
	const TextBoxStyle& style = GetTextBoxStyle();
	// Calculate the max text width and height.
	uint max_text_width = CalculateMaxTextWidth(style, max_size.width);
	uint max_text_height = CalculateMaxTextHeight(style, max_size.height);
	// Auto fit text size to the max size.
	Size text_size = _layout.AutoFitSize(Size(max_text_width, max_text_height));
	// Add padding and border to the text region.
	uint width = CalculateTextboxWidth(style, text_size.width);
	uint height = CalculateTextboxHeight(style, text_size.height);
	return Size(width, height);
}

uint _TextBox_Impl::AdjustHeightToContent(uint min_height, uint max_height, uint width) {
	const TextBoxStyle& style = GetTextBoxStyle();
	uint max_text_width = CalculateMaxTextWidth(style, width);  // Convert window width to text width;
	uint max_text_height = CalculateMaxTextHeight(style, max_height);
	Size text_size = _layout.AutoFitSize(Size(max_text_width, max_text_height));
	uint height = CalculateTextboxHeight(style, text_size.height);
	return height;
}

uint _TextBox_Impl::AdjustWidthToContent(uint min_width, uint max_width, uint height) {
	const TextBoxStyle& style = GetTextBoxStyle();
	uint max_text_width = CalculateMaxTextWidth(style, max_width);
	uint max_text_height = CalculateMaxTextHeight(style, height);
	Size text_size = _layout.AutoFitSize(Size(max_text_width, max_text_height));
	uint width = CalculateTextboxWidth(style, text_size.width);
	return width;
}

void _TextBox_Impl::SetText(wstring& text) {
	_text = &text;
	_layout.SetText(_text);
	// _layout has updated the text.
}

void _TextBox_Impl::TextUpdated() {
	_layout.TextUpdated();

	bool size_changed = ContentUpdated();
	if (size_changed) { return; }  // The window has been redrawn by parent, no need to update the region again.

	UpdateRegion(region_infinite);
}


END_NAMESPACE(WndDesign)