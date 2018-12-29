// -*- objc -*-
#import "native_window.h"


#include <assert.h>
#include "rays/bounds.h"
#include "reflex/window.h"
#include "reflex/exception.h"
#include "../view.h"
#include "event.h"
#include "window_data.h"
#import "opengl_view.h"


@implementation NativeWindow

	{
		Reflex::Window *pinstance, *ptr_for_rebind;
		OpenGLViewController* view_controller;
	}

	- (id) init
	{
		self = [super initWithFrame: UIScreen.mainScreen.bounds];
		if (!self) return nil;

		pinstance       = ptr_for_rebind = NULL;
		view_controller = [[OpenGLViewController alloc] init];

		self.rootViewController = view_controller;

		return self;
	}

	- (void) dealloc
	{
		self.rootViewController = nil;
		if (view_controller) [view_controller release];

		[self unbind];

		[super dealloc];
	}

	- (void) bind: (Reflex::Window*) instance
	{
		if (!instance)
			Reflex::argument_error(__FILE__, __LINE__);

		if (instance && instance->self->native)
			Reflex::invalid_state_error(__FILE__, __LINE__);

		instance->self->native = [self retain];

		// Reflex::Window is not constructed completely,
		// so can not call ClassWrapper::retain().
		instance->Xot::template RefCountable<>::retain();

		// defer calling ClassWrapper::retain() to rebind.
		ptr_for_rebind = instance;
	}

	- (void) rebind
	{
		if (!pinstance && ptr_for_rebind)
		{
			pinstance = ptr_for_rebind;
			pinstance->retain();

			ptr_for_rebind->Xot::template RefCountable<>::release();
			ptr_for_rebind = NULL;
		}

		assert(pinstance && !ptr_for_rebind);
	}

	- (void) unbind
	{
		[self rebind];
		if (!pinstance) return;

		if (pinstance->self->native) [pinstance->self->native release];
		pinstance->self->native = nil;

		pinstance->release();
		pinstance = NULL;
	}

	- (Reflex::Window*) instance
	{
		[self rebind];
		return pinstance;
	}

	- (void) update
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		double now = Xot::time();
		Reflex::UpdateEvent e(now, now - ptr->self->prev_time_update);
		ptr->self->prev_time_update = now;

		ptr->on_update(&e);
		if (!e.is_blocked())
			Reflex::View_update_tree(ptr->root(), e);
	}

	- (void) draw
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr || !ptr->self->redraw) return;

		ptr->self->redraw = false;

		double now = Xot::time();
		double dt  = now - ptr->self->prev_time_draw;
		double fps = 1. / dt;

		fps = ptr->self->prev_fps * 0.9 + fps * 0.1;// LPF

		ptr->self->prev_time_draw = now;
		ptr->self->prev_fps       = fps;

		Reflex::DrawEvent e(dt, fps);

		e.painter = ptr->painter();
		if (!e.painter)
			Xot::invalid_state_error(__FILE__, __LINE__);

		Rays::Bounds frame = ptr->frame();
		e.bounds.reset(0, 0, frame.width, frame.height);

		e.painter->begin();
		e.painter->clear();

		ptr->on_draw(&e);
		if (!e.is_blocked())
			Reflex::View_draw_tree(ptr->root(), e, 0, frame.move_to(0));

		e.painter->end();
	}

	- (void) frameChanged
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		Rays::Bounds b           = ptr->frame();
		Rays::Point dpos         = b.position() - ptr->self->prev_position;
		Rays::Point dsize        = b.size()     - ptr->self->prev_size;
		ptr->self->prev_position = b.position();
		ptr->self->prev_size     = b.size();

		if (dpos != 0 || dsize != 0)
		{
			Reflex::FrameEvent e(b, dpos.x, dpos.y, dsize.x, dsize.y);
			if (dpos  != 0) ptr->on_move(&e);
			if (dsize != 0)
			{
				Rays::Bounds b = ptr->frame();
				b.move_to(0, 0);

				if (ptr->painter())
					ptr->painter()->canvas(b, UIScreen.mainScreen.scale);

				if (ptr->root())
					View_set_frame(ptr->root(), b);

				ptr->on_resize(&e);
			}
		}
	}

	- (void) becomeKeyWindow
	{
		[super becomeKeyWindow];
		[self frameChanged];
	}

	- (void) keyDown: (UIEvent*) event
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		//Reflex::NativeKeyEvent e(event, Reflex::KeyEvent::DOWN);
		//ptr->on_key(&e);
	}

	- (void) keyUp: (UIEvent*) event
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		//Reflex::NativeKeyEvent e(event, Reflex::KeyEvent::UP);
		//ptr->on_key(&e);
	}

	- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		Reflex::NativePointerEvent e(touches, event, view_controller.view, Reflex::PointerEvent::DOWN);
		ptr->on_pointer(&e);
	}

	- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		Reflex::NativePointerEvent e(touches, event, view_controller.view, Reflex::PointerEvent::UP);
		ptr->on_pointer(&e);
	}

	- (void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		Reflex::NativePointerEvent e(touches, event, view_controller.view, Reflex::PointerEvent::UP);
		ptr->on_pointer(&e);
	}

	- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		Reflex::NativePointerEvent e(touches, event, view_controller.view, Reflex::PointerEvent::MOVE);
		ptr->on_pointer(&e);
	}

@end// NativeWindow
