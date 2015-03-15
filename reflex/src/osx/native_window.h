// -*- objc -*-
#import <AppKit/NSWindow.h>
#include <reflex/window.h>


@class OpenGLView;


@interface NativeWindow : NSWindow <NSWindowDelegate>

	{
		@private
			Reflex::Window::Ref* pref;
			OpenGLView* view;
			NSTimer* timer;
	}

	- (void) bind:   (Reflex::Window*) instance;

	- (void) unbind: (Reflex::Window*) instance;

	- (void) draw;

	+ (NSRect) frameRectForContentRect: (NSRect) contentRect;

@end// NativeWindow
