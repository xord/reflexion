// -*- objc -*-
#import <UIKit/UIApplication.h>
#include <reflex/application.h>


@interface ReflexAppDelegate : UIResponder <UIApplicationDelegate>

	- (void) bind: (Reflex::Application*) instance;

@end// ReflexAppDelegate
