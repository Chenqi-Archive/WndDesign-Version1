bug fixed: 
1. Provide visible region when setting canvas to child.
2. ShouldAllocateNewLayer() of _Wnd_Impl when check overlapped windows also consider z-index.
3. GetLayerVisibleRegion() as non-virtual function, but parent.getvisibleregion is virtual for desktop.
4. Redraw content when layer's resource_manager has changed.

improvements:
1. Figure export virtual DrawOn() function directly instead of exporting Create(), and figure was created by new.
2. When processing messages, the region update will be combined and the union region will be updated when message is handled.
3. Erase the target allocator when it becomes empty.