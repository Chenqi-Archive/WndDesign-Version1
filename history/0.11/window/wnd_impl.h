#pragma once

#include "wnd.h"
#include "../render/render_object.h"

#include <list>

BEGIN_NAMESPACE(WndDesign)

class _Wnd_Impl :public Wnd {

private:
	Ref _Wnd_Impl* _parent;
public:
	_Wnd_Impl(_Wnd_Impl* parent, const WndStyle* style, Handler handler);
	~_Wnd_Impl();

	void Destroy() override;


	// Style
private:
	Ref const WndStyle* _style;

public:
	const Ref WndStyle* GetStyle() const;
	void SetStyle(const WndStyle* style) override;


	// Render
private:
	RenderObject _render_object;
public:
	const Ref RenderObject* GetRenderObject() const;


	// Display the window.
	void Show() override;


	// Message
private:
	enum class Focus { Off, On, Child } _focus_state;
	const Handler _handler;
	const Handler GetHandler() const;
	void SetHandler(Handler handler) override;
	virtual void DispatchMessage(Msg msg, void* para);


	// For child windows
private:
	std::list<Ref _Wnd_Impl*> _child_wnds;
public:
	virtual bool AddChild(Ref _Wnd_Impl* child);
	virtual bool RemoveChild(Ref _Wnd_Impl* child);


	// For rendering
public:
	// Parse the style of child windows, and append to the render tree.
	virtual void ParseStyle();

};


END_NAMESPACE(WndDesign)