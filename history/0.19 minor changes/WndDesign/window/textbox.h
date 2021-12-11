#pragma once

#include "wnd_base.h"

#include "../style/textbox_style.h"

#include <string>


BEGIN_NAMESPACE(WndDesign)

using std::wstring;


// The static text box with a single format. Multiple paragraphs(lines) are permitted.
AbstractInterface ITextBox : virtual IWndBase{
	using StyleType = TextBoxStyle;
	WNDDESIGN_API static unique_ptr<ITextBox> Create(Ref<IWndBase*> parent, const StyleType& style, Ref<ObjectBase*> object = nullptr);


	///////////////////////////////////////////////////////////
	////                       Style                       ////
	///////////////////////////////////////////////////////////
	// Get the text layout region relative to the window.
	virtual const Rect GetTextRegion() const pure;

	// Conversion between point on window and point on text layout.
	const Point PointToTextPoint(Point point) const { return point - (GetTextRegion().point - point_zero); }
	const Point TextPointToPoint(Point point) const { return point + (GetTextRegion().point - point_zero); }
	const Rect RegionToTextRegion(Rect region) const { return region - (GetTextRegion().point - point_zero); }
	const Rect TextRegionToRegion(Rect region) const { return region + (GetTextRegion().point - point_zero); }


	//////////////////////////////////////////////////////////
	////                       Text                       ////
	//////////////////////////////////////////////////////////
	// The length of the text is expected to be less than text_length_max(65535).
	// You could use multiple textboxes to implement a text editor.
	static const uint text_length_max = 65535;

	virtual void SetText(const wstring& text) pure;
	virtual const wstring& GetText() const pure;
	uint GetTextLength() const { return static_cast<uint>(GetText().length()); }

	// This allow user to modify the text string with STL functions.
	// TextUpdated() should be called after the string has been modified. 
	wstring& ModifyText() { return const_cast<wstring&>(const_cast<const ITextBox&>(*this).GetText()); }
	// Update text layout with modified string, and recomposite the window.
	virtual void TextUpdated() pure;


	// For doing hit test to implement text editor.
	// All region point is relative to textbox, not text layout.
	virtual const HitTestInfo HitTestPoint(Point point) const pure; 
	virtual const HitTestInfo HitTestTextPosition(uint text_position) const pure;
	virtual void HitTestTextRange(uint text_position, uint text_length, vector<HitTestInfo>& geometry_regions) const pure;
};


END_NAMESPACE(WndDesign)