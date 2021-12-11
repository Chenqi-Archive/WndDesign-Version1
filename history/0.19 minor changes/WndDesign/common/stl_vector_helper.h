#pragma once

#include "core.h"

#include <vector>


BEGIN_NAMESPACE(WndDesign)


// Helper functions.
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

// Erase cnt elements of the vector from begin.
// Returns the iterator pointing to the next unerased element.
// If cnt is zero, returns the end.
template<class T>
inline auto erase_from_begin_by_cnt(std::vector<T>& vector, uint begin, uint cnt) {
	auto it = get_iterator_by_index(vector, begin);
	if (it != vector.end() && cnt > 0) {
		auto it_end = get_iterator_by_index(vector, begin + cnt);
		return vector.erase(it, it_end);
	} else {
		return vector.end();
	}
}

// Insert cnt elements to the vector from begin.
// Returns the iterator pointing to the first inserted element.
// If no element was inserted, returns the end.
template<class T>
inline auto insert_from_begin_by_cnt(std::vector<T>& vector, uint begin, uint cnt, const T& element) {
	if (cnt == 0) { return vector.end(); }
	auto it_begin = get_iterator_by_index(vector, begin);
	return vector.insert(it_begin, cnt, element);
}

// Resize the vector to the desired size.
// If desired size is larger than the original size, insert the specific element.
// Returns the iterator pointing to the end or the first inserted element.
template<class T>
inline auto resize_with_element(std::vector<T>& vector, uint new_size, const T& element) {
	uint old_size = static_cast<uint>(vector.size());
	vector.resize(new_size, element);
	return get_iterator_by_index(vector, old_size);
}


END_NAMESPACE(WndDesign)