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


#define REF (*pref)


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

		pref  = new Reflex::Window::Ref;
		view  = nil;
		timer = nil;

		[self setDelegate: self];
		[self setupContentView];
		[self startTimer: 60];

		return self;
	}

	- (void) dealloc
	{
		assert(pref && !REF);

		if (view) [view release];

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

		[[NSRunLoop currentRunLoop]
			addTimer: timer forMode: NSDefaultRunLoopMode];
		[[NSRunLoop currentRunLoop]
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
		assert(pref);

		if (!REF) return;

		double now = Xot::time();
		Reflex::UpdateEvent e(now, now - REF->self->prev_time_update);
		REF->self->prev_time_update = now;

		REF->on_update(&e);
		if (!e.is_blocked())
			Reflex::View_update_tree(REF->root(), e);

		if (REF->self->redraw)
		{
			[self display];
			REF->self->redraw = false;
		}
	}

	- (void) draw
	{
		assert(pref);

		if (!REF) return;

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
		if (!e.is_blocked())
			Reflex::View_draw_tree(REF->root(), e, 0, REF->frame().dup().move_to(0));

		e.painter->end();
	}

	- (BOOL) windowShouldClose: (id) sender
	{
		assert(pref);

		if (!REF) return YES;

		REF->close();
		return NO;
	}

	- (void) windowWillClose: (NSNotification*) notification
	{
		[self stopTimer];
		[self unbind: NULL];
		[self setDelegate: nil];
	}

	- (void) windowWillMove: (NSNotification*) notification
	{
		assert(pref);

		if (!REF) return;

		REF->self->prev_position = REF->frame().position();
	}

	- (void) windowDidMove: (NSNotification*) notification
	{
		[self frameChanged];
	}

	- (NSSize) windowWillResize: (NSWindow*) sender toSize: (NSSize) frameSize
	{
		assert(pref);

		if (!REF) return frameSize;

		REF->self->prev_size = REF->frame().size();

		return frameSize;
	}

	- (void) windowDidResize: (NSNotification*) notification
	{
		[self frameChanged];
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
				if (REF->root())    View_set_frame(REF->root(), b);
				REF->on_resize(&e);
			}
		}
	}

	- (void) keyDown: (NSEvent*) event
	{
		assert(pref);

		if (!REF) return;

		Reflex::NativeKeyEvent e(event, Reflex::KeyEvent::DOWN);
		REF->on_key(&e);
	}

	- (void) keyUp: (NSEvent*) event
	{
		assert(pref);

		if (!REF) return;

		Reflex::NativeKeyEvent e(event, Reflex::KeyEvent::UP);
		REF->on_key(&e);
	}

	- (void) mouseDown: (NSEvent*) event
	{
		assert(pref);

		if (!REF) return;

		Reflex::NativePointerEvent e(event, view, Reflex::PointerEvent::DOWN);
		REF->on_pointer(&e);
	}

	- (void) mouseUp: (NSEvent*) event
	{
		assert(pref);

		if (!REF) return;

		Reflex::NativePointerEvent e(event, view, Reflex::PointerEvent::UP);
		REF->on_pointer(&e);
	}

	- (void) mouseDragged: (NSEvent*) event
	{
		assert(pref);

		if (!REF) return;

		Reflex::NativePointerEvent e(event, view, Reflex::PointerEvent::MOVE);
		REF->on_pointer(&e);
	}

	- (void) mouseMoved: (NSEvent*) event
	{
		assert(pref);

		if (!REF) return;

		Reflex::NativePointerEvent e(event, view, Reflex::PointerEvent::MOVE);
		REF->on_pointer(&e);
	}

	- (void) scrollWheel: (NSEvent*) event
	{
		assert(pref);

		if (!REF) return;

		Reflex::NativeWheelEvent e(event, view);
		REF->on_wheel(&e);
	}

	+ (NSRect) frameRectForContentRect: (NSRect) contentRect
	{
		return [NSWindow
			frameRectForContentRect: contentRect
			styleMask: WINDOW_STYLE_MASK];
	}

@end// NativeWindow
