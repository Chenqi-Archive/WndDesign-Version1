#include "desktop_impl.h"


BEGIN_NAMESPACE(WndDesign)


BEGIN_NAMESPACE(Anonymous)
_Desktop_Impl _desktop_object;
END_NAMESPACE(Anonymous)


Ref _Desktop_Impl* _desktop = &_desktop_object;
Ref Desktop* desktop = reinterpret_cast<Ref Desktop*>(&_desktop_object);

END_NAMESPACE(WndDesign)