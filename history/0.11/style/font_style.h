#include "../geometry/geometry.h"
#include "../texture/texture_base.h"
#include "../resources/resources.h"

#include <string>

BEGIN_NAMESPACE(WndDesign)

// Predefined font weights.
enum class FontWeight : ushort {
    Thin = 100,
    ExtraLight = 200,
    UltraLight = 200,
    Light = 300,
    SemiLight = 350,
    Normal = 400,
    Regular = 400,
    Medium = 500,
    DemiBold = 600,
    SemiBold = 600,
    Bold = 700,
    ExtraBold = 800,
    UltraBold = 800,
    Black = 900,
    Heavy = 900,
    ExtraBlack = 950,
    UltraBlack = 950,
};

enum class FontStretch : uchar {
    Undefined,
    Ultra_condensed,
    Extra_condensed,
    Condensed,
    Semi_condensed,
    Normal,
    Medium,
    Semi_expanded,
    Expanded,
    Extra_expanded,
    Ultra_expanded
};

enum class FontItalic : uchar{
   Normal,
   Oblique,
   Italic
};


struct FontStyle {
    std::wstring font_family = L"Arial";    // 28 bytes
    Texture* effect = &built_in_resources.color_black;
    ushort font_size = 16px;
    ushort font_weight = (ushort)FontWeight::Normal;   // 1-999
    FontStretch font_stretch = FontStretch::Normal;  // 1 byte
    FontItalic font_italic = FontItalic::Normal;     // 1 byte
    bool under_line = false;  // : 1
};


END_NAMESPACE(WndDesign)