// -*- objc -*-
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>


@interface ReflexView : GLKView
@end


@interface ReflexViewController : UIViewController

	@property(nonatomic, strong, readonly) ReflexView* reflexView;

	- (ReflexView*) createReflexView;

	- (void) viewDidResize;

@end// ReflexViewController


typedef ReflexViewController* (*ReflexViewController_CreateFun) ();

void ReflexViewController_set_create_fun (ReflexViewController_CreateFun fun);

ReflexViewController* ReflexViewController_create ();
