#include "textbox_impl.h"
#include "../style/style_helper.h"


BEGIN_NAMESPACE(WndDesign)

WNDDESIGN_API Alloc TextBox* TextBox::Create(Ref WndBase* parent, const TextBoxStyle& style, const wstring& text, Handler handler) {
	_TextBox_Impl* textbox = new _TextBox_Impl(new TextBoxStyle(style), text, handler);
	textbox->DispatchMessage(Msg::Create, nullptr);
	if (parent != nullptr) { parent->AddChild(textbox); }
	return textbox;
}



_TextBox_Impl::_TextBox_Impl(Alloc TextBoxStyle* style, const wstring& text, Handler handler) :
	_WndBase_Impl(style, handler), _text(text) {
}

void _TextBox_Impl::Composite(Rect region_to_update) const {
	const TextBoxStyle& style = static_cast<const TextBoxStyle&>(GetStyle());
	FigureQueue figure_queue;
	Figure* background = new RoundedRectangle(style.border._radius,
											  style.border._width,
											  style.border._color,
											  style.background);
	figure_queue.Push(background, Rect(point_zero, GetSize()));
	Figure* text = new TextFigure(_text, style);
	figure_queue.Push(text, CalculateTextRegion(style, GetSize()));
	DrawFigureQueue(figure_queue, region_to_update);
}

void _TextBox_Impl::SetText(const wstring& text) { 
	_text = text; 
	RegionUpdated(region_infinite);
}

const wstring& _TextBox_Impl::GetText() const { 
	return _text; 
}


END_NAMESPACE(WndDesign)