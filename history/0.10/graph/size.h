#include "../common/core.h"

struct Size{
    uint width;
    uint height;

    bool IsEmpty() const {return !width || !height;}
    uint GetArea() const {return width*height;}
};