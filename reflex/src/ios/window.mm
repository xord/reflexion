// -*- objc -*-
#include "window.h"


#include "reflex/exception.h"
#import "native_window.h"


namespace Reflex
{


	WindowData&
	Window_get_data (Window* window)
	{
		if (!window)
			argument_error(__FILE__, __LINE__);

		return (WindowData&) *window->self;
	}

	const WindowData&
	Window_get_data (const Window* window)
	{
		return Window_get_data(const_cast<Window*>(window));
	}

	static NativeWindow*
	get_native (const Window* window)
	{
		NativeWindow* p = Window_get_data(const_cast<Window*>(window)).native;
		if (!p)
			invalid_state_error(__FILE__, __LINE__);

		return p;
	}


	Window::Data*
	Window_create_data ()
	{
		return new WindowData();
	}

	void
	Window_initialize (Window* window)
	{
		[[[NativeWindow alloc] init] bind: window];
	}

	void
	Window_show (Window* window)
	{
		[get_native(window) makeKeyAndVisible];
	}

	void
	Window_hide (Window* window)
	{
		not_implemented_error(__FILE__, __LINE__);
	}

	void
	Window_close (Window* window)
	{
		not_implemented_error(__FILE__, __LINE__);
	}

	void
	Window_set_title (Window* window, const char* title)
	{
		if (!title)
			argument_error(__FILE__, __LINE__);

		Window_get_data(window).title = title;
	}

	const char*
	Window_get_title (const Window& window)
	{
		return Window_get_data(&window).title.c_str();
	}

	void
	Window_set_frame (Window* window, coord x, coord y, coord width, coord height)
	{
		[get_native(window) frameChanged];
	}

	Bounds
	Window_get_frame (const Window& window)
	{
		CGRect rect = get_native(&window).frame;
		return Bounds(
			rect.origin.x,
			rect.origin.y,
			rect.size.width,
			rect.size.height);
	}


	WindowData::WindowData ()
	{
		native = nil;
	}


}// Reflex
