// -*- objc -*-
#import <AppKit/NSWindow.h>
#include <reflex/window.h>


@interface NativeWindow : NSWindow <NSWindowDelegate>

	- (void) bind: (Reflex::Window*) instance;

	- (void) draw;

	+ (NSRect) frameRectForContentRect: (NSRect) contentRect;

@end// NativeWindow
