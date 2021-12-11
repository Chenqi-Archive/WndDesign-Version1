#include "textbox_impl.h"
#include "../style/textbox_style_helper.h"

#include "../system/timer.h"
#include "../system/cursor.h"
#include "../system/ime.h"


BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API unique_ptr<ITextBox, IWndBase::Deleter> 
ITextBox::Create(Ref<IWndBase*> parent, const TextBoxStyle& style, Ref<ObjectBase*> object) {
	unique_ptr<ITextBox, IWndBase::Deleter> textbox(new _TextBox_Impl(new TextBoxStyle(style), object));
	textbox->DispatchMessage(Msg::Create, nullptr);
	if (parent != nullptr) { parent->AddChild(textbox.get()); }
	return textbox;
}



_TextBox_Impl::_TextBox_Impl(Alloc<TextBoxStyle*> style, Ref<ObjectBase*> object) :
	_WndBase_Impl(style, object),
	_text(),
	_layout(_text, GetStyle()),
	_text_region(region_empty) {
}

bool _TextBox_Impl::SetRegion(ValueTag width, ValueTag height, ValueTag left, ValueTag top) {
	TextBoxStyle& style = const_cast<TextBoxStyle&>(GetStyle());
	bool has_reset = false;
	if (!width.IsAuto()) { style.size._normal.width = width; style.auto_resize._width_auto_resize = false; has_reset = true; }
	if (!height.IsAuto()) { style.size._normal.height = height; style.auto_resize._height_auto_resize = false; has_reset = true; }
	if (!left.IsAuto()) { style.position._left = left; has_reset = true; }
	if (!top.IsAuto()) { style.position._top = top; has_reset = true; }
	if (!has_reset) { return false; }
	if (GetParent() != nullptr) { return GetParent()->UpdateChildRegion(this); }
	return false;
}

void _TextBox_Impl::SizeChanged() {
	_text_region = CalculateTextRegion(GetStyle(), GetSize());
	_layout.SetMaxSize(_text_region.size);
}

void _TextBox_Impl::Composite(Rect region_to_update) const {
	const TextBoxStyle& style = GetStyle();

	FigureQueue figure_queue;
	figure_queue.Push(
		RoundedRectangle::Create(style.border._radius, style.border._width, style.border._color, style.background), 
		Rect(point_zero, GetSize())
	);
	figure_queue.Push(TextFigure::Create(_layout), _text_region);

	DrawFigureQueue(figure_queue, region_to_update);
}

const Rect _TextBox_Impl::CalculateRegionOnParent(Size parent_size) const {
	const TextBoxStyle& style = GetStyle();
	if (!style.auto_resize.WillAutoResize()) {
		// If not auto resize, calculation is the same as WndBase.
		return CalculateActualRegion(style, parent_size);
	}

	// Auto resize.
	uint max_width= CalculateMaxTextWidth(style, parent_size);
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

void _TextBox_Impl::TextUpdated() {
	if (_text.length() > text_length_max) { ExceptionDialog(L"Text Too Long"); }

	_layout.TextUpdated();

	// Auto resize.
	const TextBoxStyle& style = GetStyle();
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