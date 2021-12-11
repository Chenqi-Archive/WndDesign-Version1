#include "table_layer.h"
#include "../window/table_impl.h"


BEGIN_NAMESPACE(WndDesign)


TableLayer::TableLayer(_Table_Impl& parent) :
	Layer(parent, region_empty, true, region_empty, point_zero, parent._WndBase_Impl::GetStyle().background) {
}


END_NAMESPACE(WndDesign)