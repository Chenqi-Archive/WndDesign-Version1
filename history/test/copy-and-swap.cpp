#include <vector>

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
	//A() :a(nullptr) {}
	A(int* a) :a(a) {}
	A(A&& right) noexcept :A(nullptr) { swap(right); }
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

int main() {
	vector<A> v;
	v.emplace_back(new int(5));
	v.emplace_back(new int(5));
	v.emplace_back(new int(5));
	v.emplace_back(new int(5));
	v.emplace_back(new int(5));
	
	v.emplace(v.begin() + 2, new int(2));
}
