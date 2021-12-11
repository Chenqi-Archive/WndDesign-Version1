#include <vector>

struct Uncopyable {
protected:
	Uncopyable() = default;
	~Uncopyable() = default;
	Uncopyable(Uncopyable&&) = default;
	Uncopyable& operator=(Uncopyable&&) = default;
private:
	Uncopyable(const Uncopyable&) = delete;
	Uncopyable& operator=(const Uncopyable&) = delete;
};

class A :Uncopyable {
public:
	A() {}
	~A() {}
};

#define Alloc
using Figure = int;
using Rect = int;
using uint = unsigned;

class FigureContainer : Uncopyable {
private:
	const Alloc Figure* figure;
	Rect initial_region;
public:
	FigureContainer(const Alloc Figure* figure, Rect initial_region)
		: figure(figure), initial_region(initial_region) {
	}
	FigureContainer(FigureContainer&&) = default;
	~FigureContainer() { delete figure; }
	const Rect GetRegion() const { return initial_region; }
	const Figure& GetFigure() const { return *figure; }
};


class FigureQueue : private std::vector<FigureContainer>, Uncopyable {
public:
	void Push(const Alloc Figure* figure, Rect initial_region) {
		push_back(FigureContainer(figure, initial_region));
	}
	uint Size() const { return static_cast<uint>(size()); }
	const FigureContainer& At(uint pos) const { return at(pos); }
	bool IsEmpty() const { return empty(); }
	void Clear() { clear(); }
};

int main() {
	FigureQueue figurequeue;

	std::vector<A> v;
	v.push_back(A());
}
