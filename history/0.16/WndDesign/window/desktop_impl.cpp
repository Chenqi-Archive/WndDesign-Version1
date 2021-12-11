#include "desktop_impl.h"

#include "../render/system_layer.h"
#include "../system/win32_api.h"
#include "../system/metrics.h"
#include "../system/timer.h"

BEGIN_NAMESPACE(WndDesign)


WNDDESIGN_API Ref<IDesktop*> IDesktop::Create(Ref<IWndBase*> parent, const StyleType& style, Ref<ObjectBase*> object) {
	return _Desktop_Impl::Get();
}


Ref<_Desktop_Impl*> _Desktop_Impl::Get() {
	static _Desktop_Impl desktop;
	return &desktop;
}

_Desktop_Impl::_Desktop_Impl() :
	_WndBase_Impl(new WndStyle(), nullptr) ,
	_timer(SetTimerSync(clean_target_interval, CleanTimerProc, this)) {
	SetCanvas(Canvas(nullptr, Rect(point_zero, GetDesktopSize())));
}

bool _Desktop_Impl::AddChild(Ref<IWndBase*> child_wnd) {
	Ref<_WndBase_Impl*> child = dynamic_cast<Ref<_WndBase_Impl*>>(child_wnd);

	Rect region = child->CalculateRegionOnParent(GetSize());

	_layers.emplace_back(*this, region, child, child->GetStyle().render);
	_layers.back().RegionUpdated(region_infinite, true);  // Redraw child, and composite.

	return true;
}

bool _Desktop_Impl::RemoveChild(Ref<IWndBase*> child_wnd) {
	for (auto it_layer = _layers.begin(); it_layer != _layers.end(); ++it_layer) {
		if (it_layer->GetWnd() == child_wnd) {
			_layers.erase(it_layer);
			return true;
		}
	}
	return false;
}

bool _Desktop_Impl::UpdateChildRegion(Ref<IWndBase*> child_wnd) {
	Ref<_WndBase_Impl*> child = dynamic_cast<Ref<_WndBase_Impl*>>(child_wnd);

	// Find the child window.
	auto it_layer = _layers.begin();
	for (; it_layer != _layers.end(); ++it_layer) {
		if (it_layer->GetWnd() == child) { break; }
	}
	if (it_layer == _layers.end()) { return false; }

	// Calculate the new region.
	Rect new_region = child->CalculateRegionOnParent(GetSize());

	if (new_region == it_layer->GetCompositeRegion()) { return false; }

	it_layer->SetRegion(new_region);
	return true;
}

void _Desktop_Impl::UpdateChildRenderStyle(Ref<IWndBase*> child_wnd) {

}

int _Desktop_Impl::MessageLoop() {
	if (_layers.empty()) { return 0; }
	return Win32::MessageLoop();
}

void _Desktop_Impl::SetCapture(_WndBase_Impl* child) {
	// Find the layer and set capture.
	auto it_layer = _layers.begin();
	for (; it_layer != _layers.end(); ++it_layer) {
		if (it_layer->GetWnd() == child) { break; }
	}
	if (it_layer == _layers.end()) { return; }

	_capture_wnd = child;
	it_layer->SetCapture();
}

void _Desktop_Impl::ReleaseCapture() {
	if (_capture_wnd == nullptr) { return; }

	auto it_layer = _layers.begin();
	for (; it_layer != _layers.end(); ++it_layer) {
		if (it_layer->GetWnd() == _capture_wnd) { break; }
	}
	if (it_layer == _layers.end()) { _capture_wnd = nullptr; return; }

	it_layer->ReleaseCapture();
	_capture_wnd = nullptr;
}


void _Desktop_Impl::CleanTimerProc(Handle timer, void* para) {
	reinterpret_cast<_Desktop_Impl*>(para)->CleanUnusedTargets();
}


END_NAMESPACE(WndDesign)