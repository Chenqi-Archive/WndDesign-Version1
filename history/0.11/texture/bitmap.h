#pragma once

#include "texture_base.h"

#include <string>

BEGIN_NAMESPACE(WndDesign)


class Bitmap : public Texture {
public:
	void Load(const std::wstring& file);
};


END_NAMESPACE(WndDesign)