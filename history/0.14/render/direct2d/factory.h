#pragma once

#include "../../common/core.h"
#include "../../geometry/geometry.h"

#include <memory>


//////////////////////////////////////////////////////////
////       Device Independent Resources Manager       ////
//////////////////////////////////////////////////////////

////  Device independent resources.

struct ID2D1Geometry;
struct ID2D1RoundedRectangleGeometry;


BEGIN_NAMESPACE(WndDesign)


Alloc ID2D1RoundedRectangleGeometry* CreateRoundedRectangleGeometry(Rect region, uint radius);


END_NAMESPACE(WndDesign)