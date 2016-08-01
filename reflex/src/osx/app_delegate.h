// -*- objc -*-
#import <AppKit/NSApplication.h>
#include <reflex/application.h>


@interface AppDelegate : NSObject <NSApplicationDelegate>

	{
		Reflex::Application::Ref* pref;
	}

	- (void) bind:   (Reflex::Application*) instance;

	- (void) unbind: (Reflex::Application*) instance;

@end// AppDelegate
