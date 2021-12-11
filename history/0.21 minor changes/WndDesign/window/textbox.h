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
	// Set the text for textbox, and the textbox will redraw.
	// !!! The text string should already be allocated and NOT temporary variable. !!!
	virtual void SetText(wstring& text) pure;

	// TextUpdated() should be called after the string has been modified, 
	//   so that the layout will be updated and the textbox will redraw. 
	virtual void TextUpdated() pure;

	// For doing hit test to implement text editor.
	// All region point is relative to textbox, not text layout.
	virtual const HitTestInfo HitTestPoint(Point point) const pure; 
	virtual const HitTestInfo HitTestTextPosition(uint text_position) const pure;
	virtual void HitTestTextRange(uint text_position, uint text_length, vector<HitTestInfo>& geometry_regions) const pure;
};


END_NAMESPACE(WndDesign)