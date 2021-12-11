#### Improvements:
1. Use unique_ptr as return value to wnd objects.
2. Cache the figure queue, and commit all the figures after all child windows has finished drawing.(Completed!)
3. Calculate the compositing region, no need to composite all per time.
4. Merge the basic functions of wnds. Like AddChild. And reorganize the window class inheritance structure. (Almost completed. Now WndBase can AddChild and RemoveChild.)
5. Point coordinate restriction(relative to layer or parent window, should not convert). (A little difficult, need run-time checking.)
6. System layer draw directly on HWNDTarget, no need to use a system tile.
7. Use device context and dxgi rendering to support more effects. And for a single system window, there only need to be one context and each tile owns a bitmap that can be selected.
8. Pre-cache before the visible region has really falled out.
9. Multiple cache region for tiles. (A bit challen)


#### Final checking:
1. Desktop has overrided all unused functions.
2. All local variables have been initialized when being constructed.
3. 



#### Test：
1. Efficiency test.
2. Pressure test.



#### Release:
1. Digital sign
2. 
