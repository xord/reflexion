// -*- objc -*-
#import <AppKit/NSApplication.h>
#include <reflex/application.h>


@interface AppDelegate : NSObject <NSApplicationDelegate>

	{
		Reflex::Application *pinstance, *ptr_for_rebind;
	}

	- (void) bind: (Reflex::Application*) instance;

	- (void) unbind;

@end// AppDelegate
