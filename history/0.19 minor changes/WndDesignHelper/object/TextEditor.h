#pragma once

#include "WndTypeBase.h"
#include "../style/editbox_style.h"
#include "../timer.h"

#include <vector>


BEGIN_NAMESPACE(WndDesignHelper)

using std::vector;


// An enhanced version of EditBox that effectively supports multiple paragraphs and scrolling.
// The TextEditor is based on Table which maintains one TextBox for each paragraph.
class TextEditor : public Table {
	struct TableFrameStyle : public TableStyle {
		TableFrameStyle(const EditBoxStyle& style) {
			size = style.size;
			position = style.position;
			auto_resize = style.auto_resize;
			render = style.render;
			border = style.border;
			background = style.background;
			grid_line.color(color_transparent).width(0);
		}
	};

public:
	TextEditor(const ObjectBase& parent, const EditBoxStyle& style) 
		:Table(parent, TableFrameStyle(style)) {

	}

private:
	vector<TextBox> _paragraphs;


};


END_NAMESPACE(WndDesignHelper)