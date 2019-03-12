// -*- objc -*-
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#include <reflex/window.h>


@interface ReflexViewController : GLKViewController

	- (void) bind: (Reflex::Window*) window;

@end// ReflexViewController
