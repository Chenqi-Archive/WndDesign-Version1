#pragma once

#include "WndDesign.h"


class SpaceView;
class Block;

class SpaceDescription : public TextBox {
private:
	SpaceView& _space;
	wstring _description;

	FadingAnimation _fading_animation;

public:
	SpaceDescription(SpaceView& space, const vector<Block*>& blocks);
	bool Handler(Msg msg, Para para) override {
		_fading_animation.TrackMsg(msg, para);
		return true;
	}
};