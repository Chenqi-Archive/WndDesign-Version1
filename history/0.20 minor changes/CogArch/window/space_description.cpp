#include "space_description.h"
#include "space_view.h"


struct SpaceDescriptionStyle : public TextBoxStyle {
	SpaceDescriptionStyle() {
		size.max(30pct, 25pct);
		auto_resize.width_auto_resize().height_auto_resize();
		position.setHorizontalCenter().top(30px);
		background.setColor(Color(ColorSet::BurlyWood, 0x7F));
		padding.setAll(10px);
		paragraph.word_wrap(WordWrap::Character).line_height(120pct);
		font.family(L"µ»œﬂ").size(20px).color(ColorSet::Black);
	}
};


SpaceDescription::SpaceDescription(SpaceView& space, const vector<Block*>& blocks) :
	TextBox(nullwnd, SpaceDescriptionStyle(), _description), _space(space), _fading_animation(*this) {

	if (blocks.size() == 0) { throw; }

	_description.assign(blocks[0]->Content(), 0, 30);

	if (blocks[0]->Content().size() > 30) { _description += L"..."; }
	if (blocks.size() > 1) { _description += L" (with other " + std::to_wstring(blocks.size() - 1) + L" blocks)"; }

	(*this)->TextUpdated();

	_space->AddChildFixed(GetWnd());
}
