// -*- objc -*-
#import <UIKit/UIApplication.h>
#include <reflex/application.h>


@interface AppDelegate : UIResponder <UIApplicationDelegate>

	- (void) bind:   (Reflex::Application*) instance;

	- (void) unbind: (Reflex::Application*) instance;

@end// AppDelegate
