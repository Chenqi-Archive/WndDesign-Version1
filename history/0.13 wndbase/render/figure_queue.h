#pragma once

#include "../common/core.h"
#include "../geometry/geometry.h"

#include "../figure/figure.h"

#include <queue>


BEGIN_NAMESPACE(WndDesign)


class FigureContainer {
private:
	const Alloc Figure* figure;
	Rect initial_region;
public:
	FigureContainer(const Alloc Figure* figure, Rect initial_region)
		: figure(figure), initial_region(initial_region){
	}
	const Rect GetRegion() const {
		return initial_region;
	}
	const Figure& GetFigure() const {
		return *figure;
	}
	void Destroy() { 
		delete figure;
		figure = nullptr; 
	}
};


class FigureQueue : private std::queue<FigureContainer> {
public:
	void Push(const Alloc Figure* figure, Rect initial_region) {
		push(FigureContainer(figure, initial_region));
	}
	const Alloc FigureContainer Pop() {
		FigureContainer figure = front(); pop();
		return figure;
	}
	bool IsEmpty() const {
		return empty();
	}
	void Clear() {
		while (!IsEmpty()) {
			FigureContainer figure = Pop();
			figure.Destroy();
		}
	}
	~FigureQueue() {
		Clear();
	}
};


END_NAMESPACE(WndDesign)