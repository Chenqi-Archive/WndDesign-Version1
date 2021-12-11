#include "desktop_impl.h"

#include "../win32/win32_api.h"
#include "../render/system_layer.h"
#include "../style/style_helper.h"
#include "../system/metrics.h"

BEGIN_NAMESPACE(WndDesign)


Ref WNDDESIGN_API Desktop* desktop = nullptr;

Ref _Desktop_Impl* _Desktop_Impl::Get() {
	static _Desktop_Impl* desktop = new _Desktop_Impl();
	return desktop;
}



_Desktop_Impl::_Desktop_Impl() :
	_WndBase_Impl(new WndStyle(), nullptr) {
	SetCanvas(Canvas(nullptr, { point_zero, GetDesktopSize() }));
	_timer_clean_target.Set(this, 0, _clean_target_interval);
}

void _Desktop_Impl::LayerUpdated(const Ref Layer* layer, Rect region) const {
	const Ref SystemWndLayer* the_layer = static_cast<const Ref SystemWndLayer*>(layer);
	if (the_layer == nullptr) { return; }
	the_layer->DrawRegion(region);
}

bool _Desktop_Impl::AddChild(Ref WndBase* child_wnd) {
	Ref _WndBase_Impl* child = dynamic_cast<Ref _WndBase_Impl*>(child_wnd);

	Rect region = CalculateActualRegion(child->GetStyle(), GetDesktopSize());
	_layers.emplace_back(this, region, child, child->GetStyle().render);
	_layers.back().RegionUpdated(region_infinite, true);  // Redraw child, and composite.
	
	child->SetParent(this);

	return true;
}

bool _Desktop_Impl::RemoveChild(Ref WndBase* child_wnd) {
	for (auto it_layer = _layers.begin(); it_layer != _layers.end(); ++it_layer) {
		if (it_layer->GetWnd() == child_wnd) {
			_layers.erase(it_layer);
			return true;
		}
	}
	return false;
}

void _Desktop_Impl::MoveChild(Ref WndBase* child_wnd) {
	Ref _WndBase_Impl* child = dynamic_cast<Ref _WndBase_Impl*>(child_wnd);

	// Find the child window.
	auto it_layer = _layers.begin();
	for (; it_layer != _layers.end(); ++it_layer) {
		if (it_layer->GetWnd() == child) { break; }
	}
	if (it_layer == _layers.end()) { return; }

	// Calculate the new region.
	const WndStyle& style = child->GetStyle();
	Rect new_region = CalculateActualRegion(style, GetSize());
	if (new_region == child->RegionOnParent()) { return; }

	it_layer->SetRegion(new_region);
}

void _Desktop_Impl::SetChildRenderStyle(Ref WndBase* child_wnd) {

}

int _Desktop_Impl::EnterMessageLoop() {
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

bool _Desktop_Impl::DispatchMessage(Msg msg, void* para) {
	if (msg == Msg::Timer && para == &_timer_clean_target) {
		for (auto& it : _layers) {
			it.CleanUnusedTargets();
		}
		return true;
	}
	return false;
}



END_NAMESPACE(WndDesign)