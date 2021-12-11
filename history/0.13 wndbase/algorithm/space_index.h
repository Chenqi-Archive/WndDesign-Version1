#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"

#include <list>

BEGIN_NAMESPACE(WndDesign)

using std::list;

// Rect indexing.

template<class _Key>
class SpaceIndex {
	using Key = _Key;
private:
	struct ObjectInfo { Key key; Rect region; };
	list<ObjectInfo> _objects;

public:
	// If there is overlapping, the latter inserted rect is on the top.
	void Insert(Key key, Rect region) {
		_objects.push_back({ key, region });
	}

	// Returns the region.
	Rect Remove(Key key) {
		for (auto it = _objects.begin(); it != _objects.end(); ++it) {
			if (it->key == key) {
				Rect region = it->region;
				_objects.erase(it);
				return region;
			}
		}
		return region_empty;
	}

	// Move to the new_region, and return the old region.
	Rect Move(Key key, Rect new_region) {
		for (auto it = _objects.begin(); it != _objects.end(); ++it) {
			if (it->key == key) {
				Rect old_region = it->region;
				it->region = new_region;
				return old_region;
			}
		}
		return region_empty;
	}

	bool IsEmpty() const {
		return _objects.empty();
	}

	void Clear() {
		_objects.clear();
	}

	// The returning list is from bottom to top.
	list<Key> QueryIntersect(Rect region) const {
		list<Key> result;
		for (auto it = _objects.begin(); it != _objects.end(); ++it) {
			if (!it->region.Intersect(region).IsEmpty()) {
				result.push_back(it->key);
			}
		}
		return result;
	}

	list<Key> QueryInside(Rect region) const {
		list<Key> result;
		for (auto it = _objects.begin(); it != _objects.end(); ++it) {
			if (region.Contains(it->region)) {
				result.push_back(it->key);
			}
		}
		return result;
	}

	// Returns the last object that contains the point
	Key QueryPoint(Point point) const {
		for (auto it = _objects.rbegin(); it != _objects.rend(); ++it) {
			if (it->region.Contains(point)) {
				return it->key;
			}
		}
		return 0;
	}
};


END_NAMESPACE(WndDesign)