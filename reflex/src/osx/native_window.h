// -*- objc -*-
#import <AppKit/NSWindow.h>
#include <reflex/window.h>


@class OpenGLView;


@interface NativeWindow : NSWindow <NSWindowDelegate>

	- (void) bind:   (Reflex::Window*) instance;

	- (void) unbind: (Reflex::Window*) instance;

	- (void) draw;

	+ (NSRect) frameRectForContentRect: (NSRect) contentRect;

@end// NativeWindow
