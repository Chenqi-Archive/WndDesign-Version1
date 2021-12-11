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
	_text(),
	_layout(_text, GetTextBoxStyle()),
	_text_region(region_empty) {
}

void _TextBox_Impl::SizeChanged() {
	_text_region = CalculateTextRegion(GetTextBoxStyle(), GetSize());
	_layout.SetMaxSize(_text_region.size);
}

void _TextBox_Impl::Composite(Rect region_to_update) const {
	const TextBoxStyle& style = GetTextBoxStyle();

	FigureQueue figure_queue;
	figure_queue.Push(
		RoundedRectangle::Create(style.border._radius, style.border._width, style.border._color, style.background), 
		Rect(point_zero, GetSize())
	);
	figure_queue.Push(TextFigure::Create(_layout), _text_region);

	DrawFigureQueue(figure_queue, region_to_update);
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

void _TextBox_Impl::TextUpdated() {
	if (_text.length() > text_length_max) { ExceptionDialog(L"Text Too Long"); }

	_layout.TextUpdated();

	bool size_changed = ContentUpdated();
	if (size_changed) { return; }  // The window has been redrawn by parent, no need to update the region again.

	RegionUpdated(region_infinite);
}


END_NAMESPACE(WndDesign)