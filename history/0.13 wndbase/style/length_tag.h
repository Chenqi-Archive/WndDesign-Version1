#pragma once

#include "../common/core.h"


BEGIN_NAMESPACE(WndDesign)


struct LengthTag {		// sizeof(LengthTag) == 4 bytes
	short length;
	bool is_auto : 1;
	bool is_percent : 1;
	bool is_center : 1;
	constexpr LengthTag operator-() const { return { -length, is_auto, is_percent, is_center }; }
private:
	constexpr operator int() const {
		return (length & 0xFFFF) | (is_auto << 16) | (is_percent << 17) | (is_center << 18);
	}
	constexpr operator uint() const {
		return (length & 0xFFFF) | (is_auto << 16) | (is_percent << 17) | (is_center << 18);
	}
public:
	constexpr LengthTag(short length, bool is_auto = false, bool is_percent = false, bool is_center = false) :
		length(length), is_auto(is_auto), is_percent(is_percent), is_center(is_center) {
	}

	// For those styles using px.
	constexpr operator uchar() const {
		if (is_auto || is_percent || is_center || length < 0) { return 0; }
		return static_cast<uchar>(length);
	}
	constexpr operator ushort() const {
		if (is_auto || is_percent || is_center || length < 0) { return 0; }
		return static_cast<uchar>(length);
	}
};


#pragma warning(disable : 4455) //literal suffix identifiers that do not start with an underscore are reserved.
constexpr LengthTag operator""px(unsigned long long number) {
	return { static_cast<short>(number), false, false, false };
}
constexpr LengthTag operator""pct(unsigned long long number) {
	return { static_cast<short>(number), false, true, false };
}
#pragma warning(default : 4455) 

constexpr LengthTag length_auto = LengthTag{ 0, true, false, false };
constexpr LengthTag position_auto = length_auto;
constexpr LengthTag position_center = LengthTag{ 0, false, false, true };


END_NAMESPACE(WndDesign)
