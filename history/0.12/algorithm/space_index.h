#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"

#include <list>

BEGIN_NAMESPACE(WndDesign)

// Rect indexing.

template<class _Key>
class SpaceIndex {
	using Key = _Key;

public:
	// If there is overlapping, the latter inserted rect is on the top.
	void Insert(Key key, Rect region);
	void Remove(Key key);

	// The returning list is from bottom to top.
	std::list<Key> QueryIntersect(Rect region);
	std::list<Key> QueryIntersect(Point position);
	std::list<Key> QueryInside(Rect region);

	// Get the union of all the rects.
	Rect GetBoundedRect() const;
};


END_NAMESPACE(WndDesign)