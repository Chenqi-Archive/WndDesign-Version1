#pragma once

#include "core.h"

BEGIN_NAMESPACE(WndDesign)


// The un-copyable base class that allows moving.
class Uncopyable {
protected:
	Uncopyable() = default;
	~Uncopyable() = default;
private:
	Uncopyable(const Uncopyable&) = delete;
	Uncopyable& operator=(const Uncopyable&) = delete;
};


END_NAMESPACE(WndDesign)