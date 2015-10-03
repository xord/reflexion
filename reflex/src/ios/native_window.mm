// -*- objc -*-
#import "native_window.h"


#include <assert.h>
#include "reflex/bounds.h"
#include "reflex/window.h"
#include "reflex/exception.h"
#include "event.h"
#include "window_data.h"
#import "opengl_view.h"


#define REF (*pref)


namespace Reflex
{

	void update_view_tree (View* v, const UpdateEvent& e);

	void draw_view_tree (View* v, const DrawEvent& e, const Point& offset, const Bounds& clip);

}// Reflex


@implementation NativeWindow

	- (id) init
	{
		self = [super initWithFrame: UIScreen.mainScreen.bounds];
		if (!self) return nil;

		pref            = new Reflex::Window::Ref;
		view_controller = [[OpenGLViewController alloc] init];

		self.rootViewController = view_controller;

		return self;
	}

	- (void) dealloc
	{
		assert(pref);

		self.rootViewController = nil;
		if (view_controller) [view_controller release];

		[self unbind: NULL];
		delete pref;

		[super dealloc];
	}

	- (void) bind: (Reflex::Window*) instance
	{
		assert(pref);

		if (instance && instance->self->native)
			Reflex::argument_error(__FILE__, __LINE__);

		if (REF)
			Reflex::invalid_state_error(__FILE__, __LINE__);

		REF = instance;
		if (REF) REF->self->native = [self retain];
	}

	- (void) unbind: (Reflex::Window*) instance
	{
		assert(pref);

		if (!REF) return;

		if (instance && instance != REF.get())
			Reflex::invalid_state_error(__FILE__, __LINE__);

		if (REF->self->native) [REF->self->native release];
		REF->self->native = nil;
		REF.reset();
	}

	- (void) update
	{
		assert(pref);

		if (!REF) return;

		double now = Xot::time();
		Reflex::UpdateEvent e(now, now - REF->self->prev_time_update);
		REF->self->prev_time_update = now;

		REF->on_update(&e);
		Reflex::update_view_tree(REF->root(), e);
	}

	- (void) draw
	{
		assert(pref);

		if (!REF || !REF->self->redraw) return;

		REF->self->redraw = false;

		double now = Xot::time();
		double dt  = now - REF->self->prev_time_draw;
		double fps = 1. / dt;

		fps                       = REF->self->prev_fps * 0.9 + fps * 0.1;// LPF
		REF->self->prev_time_draw = now;
		REF->self->prev_fps       = fps;

		Reflex::DrawEvent e(dt, fps);

		e.painter = REF->painter();
		if (!e.painter)
			Xot::invalid_state_error(__FILE__, __LINE__);

		Rays::Bounds b = REF->frame();
		e.bounds.reset(0, 0, b.width, b.height);

		e.painter->begin();
		e.painter->clear();

		REF->on_draw(&e);
		draw_view_tree(REF->root(), e, 0, REF->frame().dup().move_to(0));

		e.painter->end();
	}

	- (void) frameChanged
	{
		assert(pref);

		if (!REF) return;

		Rays::Bounds b           = REF->frame();
		Rays::Point dpos         = b.position() - REF->self->prev_position;
		Rays::Point dsize        = b.size()     - REF->self->prev_size;
		REF->self->prev_position = b.position();
		REF->self->prev_size     = b.size();

		if (dpos != 0 || dsize != 0)
		{
			Reflex::FrameEvent e(b, dpos.x, dpos.y, dsize.x, dsize.y);
			if (dpos  != 0) REF->on_move(&e);
			if (dsize != 0)
			{
				Rays::Bounds b = REF->frame();
				b.move_to(0, 0);
				if (REF->painter()) REF->painter()->canvas(b);
				if (REF->root())    REF->root()->set_frame(b);
				REF->on_resize(&e);
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
		assert(pref);

		if (!REF) return;

		//Reflex::NativeKeyEvent e(event, Reflex::KeyEvent::DOWN);
		//REF->on_key(&e);
	}

	- (void) keyUp: (UIEvent*) event
	{
		assert(pref);

		if (!REF) return;

		//Reflex::NativeKeyEvent e(event, Reflex::KeyEvent::UP);
		//REF->on_key(&e);
	}

	- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
	{
		assert(pref);

		if (!REF) return;

		Reflex::NativePointerEvent e(touches, event, view_controller.view, Reflex::PointerEvent::DOWN);
		REF->on_pointer(&e);
	}

	- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
	{
		assert(pref);

		if (!REF) return;

		Reflex::NativePointerEvent e(touches, event, view_controller.view, Reflex::PointerEvent::UP);
		REF->on_pointer(&e);
	}

	- (void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event
	{
		assert(pref);

		if (!REF) return;

		Reflex::NativePointerEvent e(touches, event, view_controller.view, Reflex::PointerEvent::UP);
		REF->on_pointer(&e);
	}

	- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
	{
		assert(pref);

		if (!REF) return;

		Reflex::NativePointerEvent e(touches, event, view_controller.view, Reflex::PointerEvent::MOVE);
		REF->on_pointer(&e);
	}

@end// NativeWindow
