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
	_text_region(region_empty) {
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

const Rect _TextBox_Impl::CalculateRegionOnParent(Size parent_size) const {
	const TextBoxStyle& style = static_cast<const TextBoxStyle&>(GetStyle());
	if (!style.auto_resize.WillAutoResize()) {
		// If not auto resize, calculation is the same as WndBase.
		return CalculateActualRegion(style, parent_size);
	}

	// Auto resize.
	uint max_width= CalculateMaxWidth(style, parent_size);
	Size text_size = _layout.AutoFitSize(max_width);
	if (style.auto_resize._width_auto_resize) {
		uint width = CalculateTextboxWidth(style, text_size.width);  // Add the padding to the width.
		const_cast<TextBoxStyle&>(style).size._normal.width.Set(width);
	}
	if (style.auto_resize._height_auto_resize) {
		uint height = CalculateTextboxHeight(style, text_size.height);
		const_cast<TextBoxStyle&>(style).size._normal.height.Set(height);
	}
	return CalculateActualRegion(style, parent_size);
}

void _TextBox_Impl::SetText(const wstring& text) {
	if (text.length() > text_length_max) { ExceptionDialog(L"Text Too Long"); }

	_text = text; 
	_layout.TextChanged();

	// Auto resize.
	const TextBoxStyle& style = static_cast<const TextBoxStyle&>(GetStyle());
	if (style.auto_resize.WillAutoResize() && GetParent() != nullptr) {
		// Inform parent to recalculate my region.
		bool size_changed = GetParent()->UpdateChildRegion(this);
		// If size is changed, the window has already finished composition, 
		//   so there's no need to update the region.
		if (size_changed) { return; }
	}

	RegionUpdated(region_infinite);
}


END_NAMESPACE(WndDesign)