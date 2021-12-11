#pragma once

#include "WndTypeBase.h"
#include "../style/editbox_style.h"
#include "../timer.h"


BEGIN_NAMESPACE(WndDesignHelper)


// An enhanced version of EditBox that effectively supports multiple paragraphs and scrolling.
// The TextEditor is based on Table and contains multiple TextBox for each paragraph.
class TextEditor : public Table {
public:
	TextEditor(const ObjectBase& parent, const EditBoxStyle& style);

private:
	EditBoxStyle::EditStyle _edit_style;

	// For drawing caret and selection when the window composites.
	void AppendFigure(FigureQueue& figure_queue) override;


};


END_NAMESPACE(WndDesignHelper)