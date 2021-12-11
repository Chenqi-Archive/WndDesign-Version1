#include "textbox_impl.h"
#include "../style/textbox_style_helper.h"


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API Alloc<ITextBox*> ITextBox::Create(Ref<IWndBase*> parent, const TextBoxStyle& style, Ref<ObjectBase*> object) {
	_TextBox_Impl* textbox = new _TextBox_Impl(new TextBoxStyle(style), object);
	textbox->DispatchMessage(Msg::Create, nullptr);
	if (parent != nullptr) { parent->AddChild(textbox); }
	return textbox;
}


_TextBox_Impl::_TextBox_Impl(Alloc<TextBoxStyle*> style, Ref<ObjectBase*> object) :
	_WndBase_Impl(style, object),
	_text(),
	_layout(_text, static_cast<const TextBoxStyle&>(GetStyle())),
	_text_region(region_empty),
	_size_normal_copy(style->size._normal) {

}

void _TextBox_Impl::SizeChanged() {
	_text_region = CalculateTextRegion(static_cast<const TextBoxStyle&>(GetStyle()), GetSize());
	_layout.SetMaxSize(_text_region.size);
}

void _TextBox_Impl::Composite(Rect region_to_update) const {
	const TextBoxStyle& style = static_cast<const TextBoxStyle&>(GetStyle());
	FigureQueue figure_queue;
	Figure* background = new RoundedRectangle(style.border._radius, style.border._width, style.border._color, style.background);
	figure_queue.Push(background, Rect(point_zero, GetSize()));
	Figure* text = new TextFigure(_layout);
	figure_queue.Push(text, _text_region);
	DrawFigureQueue(figure_queue, region_to_update);
}

bool _TextBox_Impl::AutoResize() {
	const TextBoxStyle& style = static_cast<const TextBoxStyle&>(GetStyle());
	if (!) {
		return false;  // No need to auto resize.
	}
	ValueTag width = 0px, height = 0px;
	uint width_max = _size_normal_copy.width.IsAuto() ? GetSizeMax().width : GetSize().width;
	Size text_size = _layout.AutoFitSize(width_max);
	width.length = CalculateTextboxWidth(style, text_size.width);  // Add the padding to the width.
	height.length = _size_normal_copy.height.IsAuto() ?
		CalculateTextboxHeight(style, text_size.height) : static_cast<short>(GetSize().height);
	return SetSize(width, height);
}

void _TextBox_Impl::SetText(const wstring& text) {
	if (text.length() > text_length_max) { ExceptionDialog(L"Text Too Long"); }

	_text = text; 
	_layout.TextChanged();

	bool size_changed = AutoResize();
	if (size_changed) { return; }
	// If size is changed, the window has already finished composition, 
	//   so there's no need to update the region.

	RegionUpdated(region_infinite);
}

END_NAMESPACE(WndDesign)