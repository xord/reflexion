// -*- objc -*-
#include "window.h"


#import <Cocoa/Cocoa.h>
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
		[get_native(window) makeKeyAndOrderFront: nil];
	}

	void
	Window_hide (Window* window)
	{
		NativeWindow* native = get_native(window);
		[native orderOut: native];
	}

	void
	Window_close (Window* window)
	{
		[get_native(window) close];
	}

	void
	Window_set_title (Window* window, const char* title)
	{
		if (!title)
			argument_error(__FILE__, __LINE__);

		[get_native(window) setTitle: [NSString stringWithUTF8String: title]];
	}

	const char*
	Window_get_title (const Window& window)
	{
		const WindowData& data = Window_get_data(&window);

		NSString* s = [get_native(&window) title];
		data.title_tmp = s ? [s UTF8String] : "";
		return data.title_tmp.c_str();
	}

	void
	Window_set_frame (Window* window, coord x, coord y, coord width, coord height)
	{
		NSRect frame =
			[NativeWindow frameRectForContentRect: NSMakeRect(x, y, width, height)];
		[get_native(window) setFrame: frame display: NO animate: NO];
	}

	Bounds
	Window_get_frame (const Window& window)
	{
		NativeWindow* native = get_native(&window);
		NSRect rect = [native contentRectForFrameRect: [native frame]];
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
