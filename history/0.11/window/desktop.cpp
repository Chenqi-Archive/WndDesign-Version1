#include "desktop.h"

#include <list>

BEGIN_NAMESPACE(WndDesign)


BEGIN_NAMESPACE(Anonymous)
_Desktop _desktop;
END_NAMESPACE(Anonymous)


class Wnd;
Ref Wnd* desktop = reinterpret_cast<Ref Wnd*>(&_desktop);


END_NAMESPACE(WndDesign)