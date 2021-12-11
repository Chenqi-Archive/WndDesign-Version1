#pragma once

#include "textbox.h"
#include "wnd_base_impl.h"

#include "../render/direct2d/dwrite.h"


BEGIN_NAMESPACE(WndDesign)

using Handle = void*;  // Timer handle declaration.


class _TextBox_Impl : virtual public ITextBox, public _WndBase_Impl {
	//////////////////////////////////////////////////////////
	////                  Initialization                  ////
	//////////////////////////////////////////////////////////
public:
	_TextBox_Impl(Alloc<TextBoxStyle*> style, Ref<ObjectBase*> object);
	~_TextBox_Impl() override {}


	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
public:
	const TextBoxStyle& GetTextBoxStyle() const { return static_cast<const TextBoxStyle&>(_WndBase_Impl::GetStyle()); }


	/////////////////////////////////////////////////////////
	////                    Composite                    ////
	/////////////////////////////////////////////////////////
private:
	Size AdjustSizeToContent(Size min_size, Size max_size) override;
	uint AdjustHeightToContent(uint min_height, uint max_height, uint width) override;
	uint AdjustWidthToContent(uint min_width, uint max_width, uint height) override;

public:
	void SizeChanged() override;
	void AppendFigure(FigureQueue& figure_queue, Rect region_to_update) const override;


	//////////////////////////////////////////////////////////
	////                       Text                       ////
	//////////////////////////////////////////////////////////
protected:
	Ref<const wstring*> _text;
	TextLayout _layout;
	Rect _text_region;

public:
	const Rect GetTextRegion() const override { return _text_region; }

private:
	void SetText(wstring& text) override;
	void TextUpdated() override;

public:
	const HitTestInfo HitTestPoint(Point point) const override {
		HitTestInfo info = _layout.HitTestPoint(PointToTextPoint(point));
		info.geometry_region.point = TextPointToPoint(info.geometry_region.point);
		return info;
	}
	const HitTestInfo HitTestTextPosition(uint text_position) const override {
		HitTestInfo info = _layout.HitTestTextPosition(text_position);
		info.geometry_region.point = TextPointToPoint(info.geometry_region.point);
		return info;
	}
	void HitTestTextRange(uint text_position, uint text_length, vector<HitTestInfo>& geometry_regions) const override {
		_layout.HitTestTextRange(text_position, text_length, geometry_regions);
		for (auto& info : geometry_regions) {
			info.geometry_region.point = TextPointToPoint(info.geometry_region.point);
		}
	}
};


END_NAMESPACE(WndDesign)