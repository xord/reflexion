// -*- objc -*-
#import <UIKit/UIWindow.h>
#include <reflex/window.h>


@class OpenGLViewController;


@interface NativeWindow : UIWindow

	- (void) bind: (Reflex::Window*) instance;

	- (void) update;

	- (void) draw;

	- (void) frameChanged;

	- (void) keyDown: (UIEvent*) event;

	- (void) keyUp: (UIEvent*) event;

	- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event;

	- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event;

	- (void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event;

	- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event;

@end// NativeWindow
