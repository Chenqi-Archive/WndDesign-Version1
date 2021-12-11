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
	const TextBoxStyle& GetStyle() const { return static_cast<const TextBoxStyle&>(_WndBase_Impl::GetStyle()); }

	// If the region is set explicitly, the auto_resize styles should be removed.
	bool SetRegion(ValueTag width, ValueTag height, ValueTag left, ValueTag top) override;


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

public:
	const Rect GetTextRegion() const override { return _text_region; }
private:
	const Rect CalculateRegionOnParent(Size parent_size) const override;

private:
	void TextUpdated() override;

public:
	void SetText(const wstring& text) override { _text = text; TextUpdated(); }
	const wstring& GetText() const override { return _text; }

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