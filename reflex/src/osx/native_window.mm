// -*- objc -*-
#import "native_window.h"


#include <assert.h>
#import <Cocoa/Cocoa.h>
#include "rays/bounds.h"
#include "reflex/exception.h"
#include "../view.h"
#include "event.h"
#import "opengl_view.h"


static const NSUInteger WINDOW_STYLE_MASK =
	NSTitledWindowMask |
	NSClosableWindowMask |
	NSMiniaturizableWindowMask |
	NSResizableWindowMask |
	0;//NSTexturedBackgroundWindowMask


@implementation NativeWindow

	{
		Reflex::Window *pwindow, *ptr_for_rebind;
		OpenGLView* view;
		NSTimer* timer;
	}

	- (id) init
	{
		self = [super
			initWithContentRect: NSMakeRect(0, 0, 0, 0)
			styleMask: WINDOW_STYLE_MASK
			backing: NSBackingStoreBuffered
			defer: NO];
		if (!self) return nil;

		pwindow = ptr_for_rebind = NULL;
		view    = nil;
		timer   = nil;

		[self setDelegate: self];
		[self setupContentView];
		[self startTimer: 60];

		return self;
	}

	- (void) dealloc
	{
		assert(!pwindow);

		if (view) [view release];

		[super dealloc];
	}

	- (void) bind: (Reflex::Window*) window
	{
		if (!window)
			Reflex::argument_error(__FILE__, __LINE__);

		Reflex::WindowData& data = Window_get_data(window);
		if (data.native)
			Reflex::invalid_state_error(__FILE__, __LINE__);

		data.native = [self retain];

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
		if (data.native)
		{
			[data.native release];
			data.native = nil;
		}

		pwindow->release();
		pwindow = NULL;
	}

	- (Reflex::Window*) window
	{
		[self rebind];
		return pwindow;
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
		Reflex::Window* win = self.window;
		if (!win) return;

		double now = Xot::time();
		Reflex::UpdateEvent e(now, now - win->self->prev_time_update);
		win->self->prev_time_update = now;

		win->on_update(&e);
		if (!e.is_blocked())
			Reflex::View_update_tree(win->root(), e);

		if (win->self->redraw)
		{
			[self display];
			win->self->redraw = false;
		}
	}

	- (void) draw
	{
		Reflex::Window* win = self.window;
		if (!win) return;

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
	}

	- (BOOL) windowShouldClose: (id) sender
	{
		Reflex::Window* win = self.window;
		if (!win) return YES;

		win->close();
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
		Reflex::Window* win = self.window;
		if (!win) return;

		win->self->prev_position = win->frame().position();
	}

	- (void) windowDidMove: (NSNotification*) notification
	{
		[self frameChanged];
	}

	- (NSSize) windowWillResize: (NSWindow*) sender toSize: (NSSize) frameSize
	{
		Reflex::Window* win = self.window;
		if (!win) return frameSize;

		win->self->prev_size = win->frame().size();

		return frameSize;
	}

	- (void) windowDidResize: (NSNotification*) notification
	{
		[self frameChanged];
	}

	- (void) frameChanged
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
					win->painter()->canvas(b, self.backingScaleFactor);

				if (win->root())
					View_set_frame(win->root(), b);

				win->on_resize(&e);
			}
		}
	}

	- (void) keyDown: (NSEvent*) event
	{
		Reflex::Window* win = self.window;
		if (!win) return;

		Reflex::NativeKeyEvent e(event, Reflex::KeyEvent::DOWN);
		win->on_key(&e);
	}

	- (void) keyUp: (NSEvent*) event
	{
		Reflex::Window* win = self.window;
		if (!win) return;

		Reflex::NativeKeyEvent e(event, Reflex::KeyEvent::UP);
		win->on_key(&e);
	}

	- (void) flagsChanged: (NSEvent*) event
	{
		// TODO: implement later.
	}

	- (void) mouseDown: (NSEvent*) event
	{
		Reflex::Window* win = self.window;
		if (!win) return;

		Reflex::NativePointerEvent e(event, view, Reflex::PointerEvent::DOWN);
		win->on_pointer(&e);
	}

	- (void) mouseUp: (NSEvent*) event
	{
		Reflex::Window* win = self.window;
		if (!win) return;

		Reflex::NativePointerEvent e(event, view, Reflex::PointerEvent::UP);
		win->on_pointer(&e);
	}

	- (void) mouseDragged: (NSEvent*) event
	{
		Reflex::Window* win = self.window;
		if (!win) return;

		Reflex::NativePointerEvent e(event, view, Reflex::PointerEvent::MOVE);
		win->on_pointer(&e);
	}

	- (void) mouseMoved: (NSEvent*) event
	{
		Reflex::Window* win = self.window;
		if (!win) return;

		Reflex::NativePointerEvent e(event, view, Reflex::PointerEvent::MOVE);
		win->on_pointer(&e);
	}

	- (void) scrollWheel: (NSEvent*) event
	{
		Reflex::Window* win = self.window;
		if (!win) return;

		Reflex::NativeWheelEvent e(event, view);
		win->on_wheel(&e);
	}

	+ (NSRect) frameRectForContentRect: (NSRect) contentRect
	{
		return [NSWindow
			frameRectForContentRect: contentRect
			styleMask: WINDOW_STYLE_MASK];
	}

@end// NativeWindow
