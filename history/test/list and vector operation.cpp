#include <vector>
#include <list>

using namespace std;

class Uncopyable {
protected:
	Uncopyable() = default;
	~Uncopyable() = default;
public:
	Uncopyable(const Uncopyable&) = delete;
	Uncopyable& operator=(const Uncopyable&) = delete;
};

class A : Uncopyable{
	int* a;
public:
	A() :a(nullptr) {}
	A(int* a) :a(a) {}
	A(A&& right) noexcept :A() { swap(right); }
	void swap(A& right) noexcept {
		using std::swap;
		swap(a, right.a);
	}
	void operator=(A&& right) noexcept {
		A tmp(std::move(right));
		swap(tmp);
	}
	~A() { if (a) { delete a; } }
};

namespace std {
template<>
void swap<A>(A& a, A& b) noexcept{
	a.swap(b);
}
}

using uint = unsigned;

// Helper functions.
// Get the list iterator by index.
template <class T>
inline auto get_iterator_by_index(const list<T>& list, uint index) {
	if (index <= list.size() / 2) { return std::next(list.begin(), index); }
	if (index < list.size()) { return std::prev(list.end(), list.size() - index); }
	return list.end();
}

// Get the vector iterator by index.
template <class T>
inline auto get_iterator_by_index(const vector<T>& vector, uint index) {
	if (index < vector.size()) { return vector.begin() + index; }
	return vector.end();
}

// Erase cnt elements of the list from begin.
// Returns the iterator pointing to the next intact element.
template<class T>
inline auto erase_from_begin_by_cnt(list<T>& list, uint begin, uint cnt) {
	auto it = get_iterator_by_index(list, begin);
	while (it != list.end() && cnt > 0) {
		list.erase(it++);
		cnt--;
	}
	return it;
}

// Erase cnt elements of the vector from begin.
// Returns the iterator pointing to the next intact element.
template<class T>
inline auto erase_from_begin_by_cnt(vector<T>& vector, uint begin, uint cnt) {
	auto it = get_iterator_by_index(vector, begin);
	if (it != vector.end() && cnt > 0) {
		auto it_end = get_iterator_by_index(vector, begin + cnt);
		it = vector.erase(it, it_end);
	}
	return it;
}

// In-place construct cnt elements of the vector from begin.
// It first constructs cnt new elements at the back, then swaps the elements to the correct position.
// You should overload std::swap to effectively swap two elements, and the order of the new elements should not matter.
// Returns the iterator pointing to the first inserted element.
template<class T, class... _Valty>
inline auto emplace_from_begin_by_cnt(vector<T>& vector, uint begin, uint cnt, _Valty&&... _Val) {
	if (begin > vector.size()) { begin = vector.size(); }
	if (cnt > 0) {
		int last_element = vector.size() - 1;
		vector.reserve(vector.size() + cnt);
		for (uint i = cnt; i; i--) { vector.emplace_back(std::forward<_Valty>(_Val)...); }
		for (int i = last_element; i >= static_cast<int>(begin); i--) {
			std::swap(vector[i], vector[i + cnt]);
		}
	}
	return vector.begin() + begin;
}

template<class T, class... _Valty>
inline auto emplace_from_begin_by_cnt(list<T>& list, uint begin, uint cnt, _Valty&&... _Val) {
	auto it = get_iterator_by_index(list, begin);
	while (cnt--) { list.emplace(it, std::forward<_Valty>(_Val)...); }
	return it;
}



int main() {
	vector<A> v;
	v.emplace_back(new int(1));
	v.emplace_back(new int(2));
	v.emplace_back(new int(3));
	v.emplace_back(new int(4));
	v.emplace_back(new int(5));

	emplace_from_begin_by_cnt(v, 0, 3);

	return true;

}
