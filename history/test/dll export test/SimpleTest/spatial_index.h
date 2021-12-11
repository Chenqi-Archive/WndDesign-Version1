#pragma once

#include <list>

using std::list;

using uint = unsigned;

struct Point { int x, y; };
struct Size { uint width, height; };
struct Rect { Point point; Size size; };


class SpatialIndexForPoint {
	using Value = void*;


public:
	void SetRegion(Rect region);

	void Insert(Value value, Point point);
	void Delete(Value value);

	list<Value> QueryInside(Rect rect);
};