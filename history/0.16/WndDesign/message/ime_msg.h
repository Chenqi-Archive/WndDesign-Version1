#pragma once

#include "msg.h"

#include "../system/ime.h"

BEGIN_NAMESPACE(WndDesign)


inline const ImeComposition& GetImeCompositionMsg(Para para) {
	return *reinterpret_cast<Ref<const ImeComposition*>>(para);
}


END_NAMESPACE(WndDesign)