// -*- objc -*-
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>


@interface ReflexViewController : GLKViewController
@end// ReflexViewController


typedef ReflexViewController* (*ReflexViewController_CreateFun) ();

void ReflexViewController_set_create_fun (ReflexViewController_CreateFun fun);
