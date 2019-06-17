// -*- objc -*-
#include "window.h"


#include "reflex/exception.h"
#import "view_controller.h"


@interface ReflexViewController (Bind)
	- (void) bind: (Reflex::Window*) window;
@end


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

	static ReflexViewController*
	get_vc (const Window* window)
	{
		ReflexViewController* p =
			Window_get_data(const_cast<Window*>(window)).view_controller;
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
		ReflexViewController* vc = [ReflexViewController_create() autorelease];
		if (!vc)
			reflex_error(__FILE__, __LINE__);

		[vc bind: window];
	}

	static UIWindow*
	get_window ()
	{
		UIApplication* app = UIApplication.sharedApplication;
		if (app.keyWindow)
			return app.keyWindow;
		else
		{
			UIWindow* win =
				[[UIWindow alloc] initWithFrame: UIScreen.mainScreen.bounds];
			[win makeKeyAndVisible];
			return win;
		}
	}

	static UIViewController*
	get_next_view_controller (UIViewController* vc)
	{
		assert(vc);

		if ([vc isKindOfClass: UINavigationController.class])
			return ((UINavigationController*) vc).topViewController;

		if ([vc isKindOfClass: UITabBarController.class])
			return ((UITabBarController*) vc).selectedViewController;

		if ([vc isKindOfClass: UISplitViewController.class])
		{
			UISplitViewController* split = (UISplitViewController*) vc;
			return split.viewControllers[split.viewControllers.count - 1];
		}

		return vc.presentedViewController;
	}

	static UIViewController*
	get_top_view_controller (UIViewController* vc)
	{
		assert(vc);

		UIViewController* next;
		while (next = get_next_view_controller(vc))
			vc = next;

		return vc;
	}

	void
	Window_show (Window* window)
	{
		UIViewController* vc = get_vc(window);

		UIWindow* win = get_window();
		if (!win.rootViewController)
			win.rootViewController = vc;
		else
		{
			UIViewController* top = get_top_view_controller(win.rootViewController);
			if (top.navigationController)
				[top.navigationController pushViewController: vc animated: YES];
			else
				[top presentViewController: vc animated: YES completion: nil];
		}
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

		get_vc(window).title = [[NSString alloc] initWithUTF8String: title];
	}

	const char*
	Window_get_title (const Window& window)
	{
		return [get_vc(&window).title UTF8String];
	}

	void
	Window_set_frame (Window* window, coord x, coord y, coord width, coord height)
	{
		//not_implemented_error(__FILE__, __LINE__);
	}

	Bounds
	Window_get_frame (const Window& window)
	{
		CGRect rect = get_vc(&window).view.bounds;
		return Bounds(
			rect.origin.x,
			rect.origin.y,
			rect.size.width,
			rect.size.height);
	}


	WindowData::WindowData ()
	{
		view_controller = nil;
	}


}// Reflex
