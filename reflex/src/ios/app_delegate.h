// -*- objc -*-
#import <UIKit/UIApplication.h>
#include "application.h"


@interface ReflexAppDelegate : UIResponder <UIApplicationDelegate>

	- (void) bind: (Reflex::Application*) application;

	- (BOOL) callOnStart;

@end// ReflexAppDelegate
