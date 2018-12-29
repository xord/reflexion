// -*- objc -*-
#import <AppKit/NSApplication.h>
#include <reflex/application.h>


@interface ReflexAppDelegate : NSObject <NSApplicationDelegate>

	- (void) bind: (Reflex::Application*) instance;

@end// ReflexAppDelegate
