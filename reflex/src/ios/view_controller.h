// -*- objc -*-
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>


@interface ReflexViewController : UIViewController <GLKViewDelegate>

	@property(nonatomic, strong, readonly) GLKView* glkView;

	- (GLKView*) createGLKView;

@end// ReflexViewController


typedef ReflexViewController* (*ReflexViewController_CreateFun) ();

void ReflexViewController_set_create_fun (ReflexViewController_CreateFun fun);

ReflexViewController* ReflexViewController_create ();
