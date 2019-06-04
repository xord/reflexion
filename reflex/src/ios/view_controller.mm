// -*- objc -*-
#import "view_controller.h"


#include <rays/opengl.h>
#include <rays/exception.h>
#include "reflex/exception.h"
#include "../view.h"
#include "event.h"
#include "window.h"


//#define TRANSPARENT_BACKGROUND


namespace global
{

	static ReflexViewController_CreateFun create_fun = NULL;

}// global


void
ReflexViewController_set_create_fun (ReflexViewController_CreateFun fun)
{
	global::create_fun = fun;
}

ReflexViewController*
ReflexViewController_create ()
{
	return global::create_fun
		?	global::create_fun()
		:	[[ReflexViewController alloc] init];
}


@implementation ReflexViewController

	{
		Reflex::Window *pwindow, *ptr_for_rebind;
		EAGLContext* context;
	}

	- (id) init
	{
		self = [super init];
		if (!self) return nil;

		pwindow = ptr_for_rebind = NULL;
		context = nil;

		return self;
	}

	- (void) dealloc
	{
		[self unbind];
		[self cleanupContext];

		[super dealloc];
	}

	- (void) bind: (Reflex::Window*) window
	{
		if (!window)
			Reflex::argument_error(__FILE__, __LINE__);

		Reflex::WindowData& data = Window_get_data(window);
		if (data.view_controller)
			Reflex::invalid_state_error(__FILE__, __LINE__);

		data.view_controller = [self retain];

		// Reflex::Window is not constructed completely,
		// so can not call ClassWrapper::retain().
		window->Xot::template RefCountable<>::retain();

		// defer calling ClassWrapper::retain() to rebind.
		ptr_for_rebind = window;
	}

	- (void) rebind
	{
		if (!pwindow && ptr_for_rebind)
		{
			pwindow = ptr_for_rebind;
			pwindow->retain();

			ptr_for_rebind->Xot::template RefCountable<>::release();
			ptr_for_rebind = NULL;
		}

		assert(pwindow && !ptr_for_rebind);
	}

	- (void) unbind
	{
		[self rebind];
		if (!pwindow) return;

		Reflex::WindowData& data = Window_get_data(pwindow);
		if (data.view_controller)
		{
			[data.view_controller release];
			data.view_controller = nil;
		}

		pwindow->release();
		pwindow = NULL;
	}

	- (Reflex::Window*) window
	{
		[self rebind];
		return pwindow;
	}

	- (void) viewDidLoad
	{
		[super viewDidLoad];

		context = (EAGLContext*) Rays::get_offscreen_context();
		if (!context)
		{
			Reflex::reflex_error(
				__FILE__, __LINE__, "failed to setup OpenGL context.");
		}

		GLKView* view = (GLKView*) self.view;
		view.context              = context;
		view.multipleTouchEnabled = YES;
		view.drawableDepthFormat  = GLKViewDrawableDepthFormat24;
		//view.drawableMultisample  = GLKViewDrawableMultisample4X;

		self.preferredFramesPerSecond = 60;

		[self activateContext];
	}

	- (void) didReceiveMemoryWarning
	{
		[super didReceiveMemoryWarning];

		if ([self isViewLoaded] && !self.view.window)
		{
			self.view = nil;
			[self cleanupContext];
		}
	}

	- (void) activateContext
	{
		[EAGLContext setCurrentContext: context];
	}

	- (void) cleanupContext
	{
		[self activateContext];

		if (context && [EAGLContext currentContext] == context)
		{
			[EAGLContext setCurrentContext: nil];
			context = nil;
		}
	}

	- (void) update
	{
		Reflex::Window* win = self.window;
		if (!win) return;

		double now = Xot::time();
		Reflex::UpdateEvent e(now, now - win->self->prev_time_update);
		win->self->prev_time_update = now;

		win->on_update(&e);
		if (!e.is_blocked())
			Reflex::View_update_tree(win->root(), e);
	}

	- (void) glkView: (GLKView*) view drawInRect: (CGRect) rect
	{
		Reflex::Window* win = self.window;
		if (!win || !win->self->redraw)
			return;

		[self activateContext];

		double now = Xot::time();
		double dt  = now - win->self->prev_time_draw;
		double fps = 1. / dt;

		fps = win->self->prev_fps * 0.9 + fps * 0.1;// LPF

		win->self->prev_time_draw = now;
		win->self->prev_fps       = fps;

		Reflex::DrawEvent e(dt, fps);

		e.painter = win->painter();
		if (!e.painter)
			Xot::invalid_state_error(__FILE__, __LINE__);

		Rays::Bounds frame = win->frame();
		e.bounds.reset(0, 0, frame.width, frame.height);

		e.painter->begin();
		e.painter->clear();

		win->on_draw(&e);
		if (!e.is_blocked())
			Reflex::View_draw_tree(win->root(), e, 0, frame.move_to(0));

		e.painter->end();

		win->self->redraw = false;
	}

	- (void) viewWillLayoutSubviews
	{
		Reflex::Window* win = self.window;
		if (!win) return;

		Rays::Bounds b           = win->frame();
		Rays::Point dpos         = b.position() - win->self->prev_position;
		Rays::Point dsize        = b.size()     - win->self->prev_size;
		win->self->prev_position = b.position();
		win->self->prev_size     = b.size();

		if (dpos != 0 || dsize != 0)
		{
			Reflex::FrameEvent e(b, dpos.x, dpos.y, dsize.x, dsize.y);
			if (dpos  != 0) win->on_move(&e);
			if (dsize != 0)
			{
				Rays::Bounds b = win->frame();
				b.move_to(0, 0);

				if (win->painter())
					win->painter()->canvas(b, UIScreen.mainScreen.scale);

				if (win->root())
					View_set_frame(win->root(), b);

				win->on_resize(&e);
			}
		}
	}

	- (void) keyDown: (UIEvent*) event
	{
		//[self interpretKeyEvents: [NSArray arrayWithObject: event]];

		Reflex::Window* win = self.window;
		if (!win) return;

		//Reflex::NativeKeyEvent e(event, Reflex::KeyEvent::DOWN);
		//win->on_key(&e);
	}

	- (void) keyUp: (UIEvent*) event
	{
		Reflex::Window* win = self.window;
		if (!win) return;

		//Reflex::NativeKeyEvent e(event, Reflex::KeyEvent::UP);
		//win->on_key(&e);
	}

	- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
	{
		Reflex::Window* win = self.window;
		if (!win) return;

		Reflex::NativePointerEvent e(
			touches, event, self.view, Reflex::PointerEvent::DOWN);
		win->on_pointer(&e);
	}

	- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
	{
		Reflex::Window* win = self.window;
		if (!win) return;

		Reflex::NativePointerEvent e(
			touches, event, self.view, Reflex::PointerEvent::UP);
		win->on_pointer(&e);
	}

	- (void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event
	{
		Reflex::Window* win = self.window;
		if (!win) return;

		Reflex::NativePointerEvent e(
			touches, event, self.view, Reflex::PointerEvent::UP);
		win->on_pointer(&e);
	}

	- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
	{
		Reflex::Window* win = self.window;
		if (!win) return;

		Reflex::NativePointerEvent e(
			touches, event, self.view, Reflex::PointerEvent::MOVE);
		win->on_pointer(&e);
	}

@end// ReflexViewController
