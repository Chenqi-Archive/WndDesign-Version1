#pragma once

#include "texture_base.h"

#include <string>

BEGIN_NAMESPACE(WndDesign)


class Image : public Texture {
public:
	Image(const std::wstring& file);
	bool Load(const std::wstring& file);
};


END_NAMESPACE(WndDesign)