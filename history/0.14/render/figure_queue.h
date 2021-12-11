#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"
#include "../figure/figure.h"

#include <vector>


BEGIN_NAMESPACE(WndDesign)


class FigureContainer : Uncopyable{
private:
	const Alloc Figure* figure;
	Rect initial_region;
public:
	FigureContainer(const Alloc Figure* figure, Rect initial_region)
		: figure(figure), initial_region(initial_region) {
	}
	FigureContainer(FigureContainer&& right) noexcept :
		figure(right.figure), initial_region(right.initial_region) {
		right.figure = nullptr;
	}
	~FigureContainer() { delete figure; }
	const Rect GetRegion() const { return initial_region; }
	const Figure& GetFigure() const { return *figure; }
};


class FigureQueue : private std::vector<FigureContainer>{
public:
	void Push(const Alloc Figure* figure, Rect initial_region) {
		push_back(FigureContainer(figure, initial_region));
	}
	uint Size() const { return static_cast<uint>(size()); }
	const FigureContainer& Get(uint pos) const { return at(pos); }
	bool IsEmpty() const { return empty(); }
	void Clear() { clear(); }
};


END_NAMESPACE(WndDesign)