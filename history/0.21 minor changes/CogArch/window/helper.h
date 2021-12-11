#pragma once

#include "../WndDesign.h"


inline int BoundNumberToNearestLatticeNumber(int number, uint lattice_length) {
	int length = static_cast<int>(lattice_length);
	if (length <= 1) { return number; }
	int remainder = number % length;
	if (remainder < 0) { remainder += length; }
	number -= remainder;
	if (remainder > length / 2) { number += length; }
	return number;
}


inline Point BoundPointToNearestLatticePoint(Point point, Size grid_size) {
	return Point(
		BoundNumberToNearestLatticeNumber(point.x, grid_size.width),
		BoundNumberToNearestLatticeNumber(point.y, grid_size.height)
	);
}


inline Rect BoundingRegionOfCircle(Point center, uint radius) {
	return Rect(
		center - Vector(static_cast<int>(radius), static_cast<int>(radius)),
		Size(radius * 2, radius * 2)
	);
}


inline Vector operator*(Vector vector, float ratio) {
	return Vector(static_cast<int>(vector.x * ratio), static_cast<int>(vector.y * ratio));
}

inline uint SquareLength(Vector vector) {
	return	square(vector.x) + square(vector.y);
}