// -*- objc -*-
#import "native_window.h"


#include <assert.h>
#import <Cocoa/Cocoa.h>
#include "rays/bounds.h"
#include "reflex/window.h"
#include "reflex/exception.h"
#include "../view.h"
#include "event.h"
#include "window_data.h"
#import "opengl_view.h"


static const NSUInteger WINDOW_STYLE_MASK =
	NSTitledWindowMask |
	NSClosableWindowMask |
	NSMiniaturizableWindowMask |
	NSResizableWindowMask |
	0;//NSTexturedBackgroundWindowMask


@implementation NativeWindow

	- (id) init
	{
		self = [super
			initWithContentRect: NSMakeRect(0, 0, 0, 0)
			styleMask: WINDOW_STYLE_MASK
			backing: NSBackingStoreBuffered
			defer: NO];
		if (!self) return nil;

		pinstance      =
		ptr_for_rebind = NULL;
		view           = nil;
		timer          = nil;

		[self setDelegate: self];
		[self setupContentView];
		[self startTimer: 60];

		return self;
	}

	- (void) dealloc
	{
		assert(!pinstance);

		if (view) [view release];

		[super dealloc];
	}

	- (void) bind: (Reflex::Window*) instance
	{
		if (!instance)
			Reflex::argument_error(__FILE__, __LINE__);

		if (instance && instance->self->native)
			Reflex::invalid_state_error(__FILE__, __LINE__);

		instance->self->native = [self retain];

		// Reflex::Application is not constructed completely,
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

	- (void) setupContentView
	{
		NSRect rect = [self contentRectForFrameRect: [self frame]];
		rect.origin.x = rect.origin.y = 0;
		view = [[OpenGLView alloc] initWithFrame: rect];
		[self setContentView: view];
	}

	- (void) startTimer: (int) fps
	{
		[self stopTimer];

		if (fps <= 0) return;

		timer = [[NSTimer
			timerWithTimeInterval: 1. / (double) fps
			target: self
			selector: @selector(update:)
			userInfo: nil
			repeats: YES] retain];
		if (!timer) return;

		[[NSRunLoop mainRunLoop]
			addTimer: timer forMode: NSDefaultRunLoopMode];
		[[NSRunLoop mainRunLoop]
			addTimer: timer forMode: NSEventTrackingRunLoopMode];
	}

	- (void) stopTimer
	{
		if (!timer) return;

		[timer invalidate];
		timer = nil;
	}

	- (void) update: (NSTimer*) t
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		double now = Xot::time();
		Reflex::UpdateEvent e(now, now - ptr->self->prev_time_update);
		ptr->self->prev_time_update = now;

		ptr->on_update(&e);
		if (!e.is_blocked())
			Reflex::View_update_tree(ptr->root(), e);

		if (ptr->self->redraw)
		{
			[self display];
			ptr->self->redraw = false;
		}
	}

	- (void) draw
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		double now = Xot::time();
		double dt  = now - ptr->self->prev_time_draw;
		double fps = 1. / dt;

		fps                       = ptr->self->prev_fps * 0.9 + fps * 0.1;// LPF
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

	- (BOOL) windowShouldClose: (id) sender
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return YES;

		ptr->close();
		return NO;
	}

	- (void) windowWillClose: (NSNotification*) notification
	{
		[self stopTimer];
		[self unbind];
		[self setDelegate: nil];
	}

	- (void) windowWillMove: (NSNotification*) notification
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		ptr->self->prev_position = ptr->frame().position();
	}

	- (void) windowDidMove: (NSNotification*) notification
	{
		[self frameChanged];
	}

	- (NSSize) windowWillResize: (NSWindow*) sender toSize: (NSSize) frameSize
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return frameSize;

		ptr->self->prev_size = ptr->frame().size();

		return frameSize;
	}

	- (void) windowDidResize: (NSNotification*) notification
	{
		[self frameChanged];
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
					ptr->painter()->canvas(b, self.backingScaleFactor);

				if (ptr->root())
					View_set_frame(ptr->root(), b);

				ptr->on_resize(&e);
			}
		}
	}

	- (void) keyDown: (NSEvent*) event
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		Reflex::NativeKeyEvent e(event, Reflex::KeyEvent::DOWN);
		ptr->on_key(&e);
	}

	- (void) keyUp: (NSEvent*) event
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		Reflex::NativeKeyEvent e(event, Reflex::KeyEvent::UP);
		ptr->on_key(&e);
	}

	- (void) flagsChanged: (NSEvent*) event
	{
		// TODO: implement later.
	}

	- (void) mouseDown: (NSEvent*) event
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		Reflex::NativePointerEvent e(event, view, Reflex::PointerEvent::DOWN);
		ptr->on_pointer(&e);
	}

	- (void) mouseUp: (NSEvent*) event
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		Reflex::NativePointerEvent e(event, view, Reflex::PointerEvent::UP);
		ptr->on_pointer(&e);
	}

	- (void) mouseDragged: (NSEvent*) event
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		Reflex::NativePointerEvent e(event, view, Reflex::PointerEvent::MOVE);
		ptr->on_pointer(&e);
	}

	- (void) mouseMoved: (NSEvent*) event
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		Reflex::NativePointerEvent e(event, view, Reflex::PointerEvent::MOVE);
		ptr->on_pointer(&e);
	}

	- (void) scrollWheel: (NSEvent*) event
	{
		Reflex::Window* ptr = [self instance];
		if (!ptr) return;

		Reflex::NativeWheelEvent e(event, view);
		ptr->on_wheel(&e);
	}

	+ (NSRect) frameRectForContentRect: (NSRect) contentRect
	{
		return [NSWindow
			frameRectForContentRect: contentRect
			styleMask: WINDOW_STYLE_MASK];
	}

@end// NativeWindow
