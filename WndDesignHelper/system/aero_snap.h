#pragma once

#include "../WndDesign.h"
#include "../style/border_position.h"

BEGIN_NAMESPACE(WndDesignHelper)


void SystemWindowDraggingLeftDown(ObjectBase& object, Point point);


void SystemWindowBorderResizingLeftDown(ObjectBase& object, Point point, BorderPosition border_position);


END_NAMESPACE(WndDesignHelper)