#pragma once

#include "core.h"


BEGIN_NAMESPACE(WndDesign)


// Get the normal iterator from reverse iterator(must be valid) who points to the same element.
// Directly use base() might be misleading.
// When you want to insert a new element that follows(in normal order) the current element 
//   a reverse iterator points to, use base() directly.
template<class reverse_iterator>
auto get_cooresponding_iterator(reverse_iterator it_reverse) {
	return it_reverse.base() - 1;
}


END_NAMESPACE(WndDesign)