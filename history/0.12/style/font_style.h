#pragma once

#include "../geometry/geometry.h"
#include "../texture/texture_base.h"
#include "../resources/resources.h"

#include <string>

BEGIN_NAMESPACE(WndDesign)

using std::wstring;

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
    wstring family = L"Arial";     // 28 bytes
    Color32 color = ColorSet::Black;              
    uchar size : 8;                     // 0-255
    ushort weight : 10;                 // 1-999
    FontStretch stretch : 4;    
    FontItalic italic : 2;      
    bool underline : 1;
    bool strikeline : 1;

    FontStyle() {
        size = 16px;
        weight = (ushort)FontWeight::Normal;
        stretch = FontStretch::Normal;
        italic = FontItalic::Normal;
        underline = false;
        strikeline = false;
    }
};


END_NAMESPACE(WndDesign)