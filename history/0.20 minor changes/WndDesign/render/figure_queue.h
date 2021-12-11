#pragma once

#include "../common/core.h"
#include "../common/feature.h"
#include "../geometry/geometry.h"
#include "../figure/figure.h"

#include <vector>
#include <memory>


BEGIN_NAMESPACE(WndDesign)


class FigureContainer : Uncopyable {
private:
	std::unique_ptr<const Figure> figure;
	Rect initial_region;
public:
	FigureContainer(std::unique_ptr<const Figure> figure, Rect initial_region) : 
		figure(std::move(figure)), initial_region(initial_region) {}
	FigureContainer(FigureContainer&& right) noexcept : 
		figure(std::move(right.figure)), initial_region(right.initial_region) {}
	const Rect GetRegion() const { return initial_region; }
	const Figure& GetFigure() const { return *figure; }
};


class FigureQueue : public std::vector<FigureContainer> {
public:
	void append(Alloc<const Figure*> figure, Rect initial_region) {
		emplace_back(std::unique_ptr<const Figure>(figure), initial_region);
	}
};


END_NAMESPACE(WndDesign)