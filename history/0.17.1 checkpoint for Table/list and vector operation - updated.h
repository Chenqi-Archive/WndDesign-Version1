#pragma once

#include "core.h"

#include <list>
#include <vector>


BEGIN_NAMESPACE(WndDesign)


// Helper functions.
// Used by Table.


// Get the list iterator by index.
// If index is larger than the size, returns the end.
template <class T>
inline auto get_iterator_by_index(const std::list<T>& list, uint index) {
	if (index <= list.size() / 2) { return std::next(list.begin(), index); }
	if (index < list.size()) { return std::prev(list.end(), list.size() - index); }
	return list.end();
}
// Non-const version.
template <class T>
inline auto get_iterator_by_index(std::list<T>& list, uint index) {
	if (index <= list.size() / 2) { return std::next(list.begin(), index); }
	if (index < list.size()) { return std::prev(list.end(), list.size() - index); }
	return list.end();
}

// Get the vector iterator by index.
// If index is larger than the size, returns the end.
template <class T>
inline auto get_iterator_by_index(const std::vector<T>& vector, uint index) {
	if (index < vector.size()) { return vector.begin() + index; }
	return vector.end();
}
// Non-const version.
template <class T>
inline auto get_iterator_by_index(std::vector<T>& vector, uint index) {
	if (index < vector.size()) { return vector.begin() + index; }
	return vector.end();
}

// Erase cnt elements of the list from begin.
// Returns the iterator pointing to the next unerased element.
// If cnt is zero, returns the end.
template<class T>
inline auto erase_from_begin_by_cnt(std::list<T>& list, uint begin, uint cnt) {
	if (begin >= list.size()) { return list.end(); }
	if (begin + cnt >= list.size()) {  // begin < list.size()
		for (uint num = list.size() - begin; num; num--) {
			list.pop_back();
		}
		return list.end();
	}
	if (begin < list.size() - begin - cnt) {
		auto it_begin = std::next(list.begin(), begin);
		while (cnt--) {
			list.erase(it_begin++);
		}
		return it_begin;
	} else {
		auto it_end = std::prev(list.end(), list.size() - begin - cnt + 1);
		while (cnt--) {
			list.erase(it_end--);
		}
		return it_end;
	}
}

// Erase cnt elements of the vector from begin.
// Returns the iterator pointing to the next unerased element.
// If cnt is zero, returns the end.
template<class T>
inline auto erase_from_begin_by_cnt(std::vector<T>& vector, uint begin, uint cnt) {
	auto it = get_iterator_by_index(vector, begin);
	if (it != vector.end() && cnt > 0) {
		auto it_end = get_iterator_by_index(vector, begin + cnt);
		it = vector.erase(it, it_end);
		return it;
	} else {
		return vector.end();
	}
}

// In-place construct cnt elements of the list from begin.
// Returns the iterator pointing to the first inserted element.
template<class T, class... _Valty>
inline auto emplace_from_begin_by_cnt(std::list<T>& list, uint begin, uint cnt, _Valty&&... _Val) {
	if (cnt == 0) { return list.end(); }
	auto it = get_iterator_by_index(list, begin);
	while (cnt--) { list.emplace(it, std::forward<_Valty>(_Val)...); }
	return it;
}

// In-place construct cnt elements of the vector from begin.
// It first constructs cnt new elements at the back, then swaps the elements to the correct position.
// You should overload std::swap to effectively swap two elements, and the order of the new elements should not matter.
// If the order of new elements does matter, use std::rotate() instead of swapping manually.
// Returns the iterator pointing to the first emplaced element.
template<class T, class... _Valty>
inline auto emplace_from_begin_by_cnt(std::vector<T>& vector, uint begin, uint cnt, _Valty&&... _Val) {
	if (begin > vector.size()) { begin = vector.size(); }
	if (cnt == 1) {
		vector.emplace(vector.begin() + begin, std::forward<_Valty>(_Val)...);
	} else if (cnt > 1) {
		int last_old_element = vector.size() - 1;
		vector.reserve(vector.size() + cnt);
		for (uint i = cnt; i; i--) { vector.emplace_back(std::forward<_Valty>(_Val)...); }
		for (int i = last_old_element; i >= static_cast<int>(begin); i--) {  // Use int in case of overflowing at 0.
			std::swap(vector[i], vector[i + cnt]);
		}
	} else { // cnt == 0
		return vector.end();  // No element is inserted, return the end of the vector.
	}
	return vector.begin() + begin;
}


END_NAMESPACE(WndDesign)