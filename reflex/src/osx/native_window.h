// -*- objc -*-
#import <AppKit/NSWindow.h>
#include "window.h"


@interface NativeWindow : NSWindow <NSWindowDelegate>

	- (void) bind: (Reflex::Window*) window;

	- (void) draw;

	+ (NSRect) frameRectForContentRect: (NSRect) contentRect;

@end// NativeWindow
