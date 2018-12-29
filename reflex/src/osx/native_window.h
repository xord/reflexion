// -*- objc -*-
#import <AppKit/NSWindow.h>
#include <reflex/window.h>


@class OpenGLView;


@interface NativeWindow : NSWindow <NSWindowDelegate>

	{
		Reflex::Window *pinstance, *ptr_for_rebind;
		OpenGLView* view;
		NSTimer* timer;
	}

	- (void) bind: (Reflex::Window*) instance;

	- (void) unbind;

	- (void) draw;

	+ (NSRect) frameRectForContentRect: (NSRect) contentRect;

@end// NativeWindow
