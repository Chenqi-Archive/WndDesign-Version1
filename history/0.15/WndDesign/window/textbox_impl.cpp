#include "textbox_impl.h"
#include "../style/style_helper.h"


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

void _TextBox_Impl::SetCanvas(const Canvas& canvas) {
	bool size_changed = (GetSize() != canvas.GetSize());
	_WndBase_Impl::SetCanvas(canvas);

	if (!size_changed) { return; }

	_text_region = CalculateTextRegion(static_cast<const TextBoxStyle&>(GetStyle()), GetSize());
	_layout.SetMaxSize(_text_region.size);

}

void _TextBox_Impl::Composite(Rect region_to_update) const {
	const TextBoxStyle& style = static_cast<const TextBoxStyle&>(GetStyle());
	FigureQueue figure_queue;
	Figure* background = new RoundedRectangle(style.border._radius,
											  style.border._width,
											  style.border._color,
											  style.background);
	figure_queue.Push(background, Rect(point_zero, GetSize()));
	Figure* text = new TextFigure(_layout);
	figure_queue.Push(text, _text_region);
	DrawFigureQueue(figure_queue, region_to_update);
}

void _TextBox_Impl::SetText(const wstring& text) { 
	_text = text; 
	_layout.TextChanged();

	// Extended style.

	// Auto resize.

	RegionUpdated(region_infinite);
}

const wstring& _TextBox_Impl::GetText() const { 
	return _text; 
}


END_NAMESPACE(WndDesign)