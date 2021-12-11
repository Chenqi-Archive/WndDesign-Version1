#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"


BEGIN_NAMESPACE(WndDesign)


class ValueTag {
public:
	enum class Tag { Pixel, Percent, Center, Auto };

private:
	int _value;
	Tag _tag;

public:
	explicit constexpr ValueTag(int value, Tag tag = Tag::Pixel) : _value(value), _tag(tag) {}
	explicit constexpr ValueTag(uint value, Tag tag = Tag::Pixel) : _value(static_cast<int>(value)), _tag(tag) {}

	constexpr void Set(int value, Tag tag = Tag::Pixel) { _value = value; _tag = tag; }
	constexpr void Set(uint value, Tag tag = Tag::Pixel) { _value = static_cast<int>(value); _tag = tag; }

	constexpr bool IsPixel() const { return _tag == Tag::Pixel; }
	constexpr bool IsPercent() const { return _tag == Tag::Percent; }
	constexpr bool IsCenter() const { return _tag == Tag::Center; }
	constexpr bool IsAuto() const { return _tag == Tag::Auto; }

	constexpr int AsSigned() const { return _value; }
	constexpr uint AsUnsigned() const { return _value >= 0 ? static_cast<uint>(_value) : 0; }

	constexpr void ConvertToPixel(uint entire_length) {
		if (_tag == Tag::Percent) {
			_value = _value * static_cast<int>(entire_length) / 100;
			_tag = Tag::Pixel;
		}
	}
};



#pragma warning (push)
#pragma warning (disable : 4455)   // literal suffix identifiers that do not start with an underscore are reserved.
constexpr ValueTag operator""px(unsigned long long number) {
	return ValueTag(static_cast<int>(number), ValueTag::Tag::Pixel);
}
constexpr ValueTag operator""pct(unsigned long long number) {
	return ValueTag(static_cast<int>(number), ValueTag::Tag::Percent);
}
#pragma warning (pop)


constexpr ValueTag px(int number) {
	return ValueTag(number, ValueTag::Tag::Pixel);
}
constexpr ValueTag pct(int number) {
	return ValueTag(number, ValueTag::Tag::Percent);
}
constexpr ValueTag px(uint number) {
	return ValueTag(static_cast<int>(number), ValueTag::Tag::Pixel);
}
constexpr ValueTag pct(uint number) {
	return ValueTag(static_cast<int>(number), ValueTag::Tag::Percent);
}


constexpr ValueTag length_min_tag = ValueTag(length_min, ValueTag::Tag::Pixel);
constexpr ValueTag length_max_tag = ValueTag(length_max, ValueTag::Tag::Pixel);
constexpr ValueTag length_auto = ValueTag(0, ValueTag::Tag::Auto);

constexpr ValueTag position_min_tag = ValueTag(position_min, ValueTag::Tag::Pixel);
constexpr ValueTag position_max_tag = ValueTag(position_max, ValueTag::Tag::Pixel);
constexpr ValueTag position_center = ValueTag(0, ValueTag::Tag::Center);
constexpr ValueTag position_auto = length_auto;


END_NAMESPACE(WndDesign)