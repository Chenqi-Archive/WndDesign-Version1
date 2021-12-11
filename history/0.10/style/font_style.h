#include "../common/core.h"

#include <string>

// Predefined font weights.
enum class FontWeight: ushort{
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



struct FontStyle{
    std::wstring font_family;
    ushort font_weight;   // 1-999
    
};