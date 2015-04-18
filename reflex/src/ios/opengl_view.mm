// -*- objc -*-
#import "opengl_view.h"


#include <rays/opengl.h>
#include <rays/exception.h>
#include "reflex/exception.h"
#import "native_window.h"


//#define TRANSPARENT_BACKGROUND


@implementation OpenGLViewController

	- (void) viewDidLoad
	{
		[super viewDidLoad];

		context = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES1];
		if (!context)
			Reflex::reflex_error(__FILE__, __LINE__, "failed to create OpenGL context.");

		GLKView* view = (GLKView*) self.view;
		view.context             = context;
		view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
		view.drawableMultisample = GLKViewDrawableMultisample4X;

		self.preferredFramesPerSecond = 60;

		[self setupOpenGL];
	}

	- (void) dealloc
	{
		[self teardownOpenGL];
		[super dealloc];
	}

	- (void) didReceiveMemoryWarning
	{
		[super didReceiveMemoryWarning];

		if ([self isViewLoaded] && self.view.window == nil)
		{
			self.view = nil;
			[self teardownOpenGL];
		}
	}

	- (void) activateContext
	{
		[EAGLContext setCurrentContext: context];
	}

	- (void) setupOpenGL
	{
		[self activateContext];
	}

	- (void) teardownOpenGL
	{
		[self activateContext];

		if (context && [EAGLContext currentContext] == context)
		{
			[EAGLContext setCurrentContext: nil];
			context = nil;
		}
	}

	- (void) update
	{
		NativeWindow* win = (NativeWindow*) self.view.window;
		if (!win) return;

		[win update];
	}

	- (void) glkView: (GLKView*) view drawInRect: (CGRect) rect
	{
		NativeWindow* win = (NativeWindow*) self.view.window;
		if (!win) return;

		[self activateContext];
		[win draw];
	}

	- (void) keyDown: (UIEvent*) event
	{
		//[self interpretKeyEvents: [NSArray arrayWithObject: event]];

		NativeWindow* win = (NativeWindow*) self.view.window;
		if (!win) return;

		[win keyDown: event];
	}

	- (void) keyUp: (UIEvent*) event
	{
		NativeWindow* win = (NativeWindow*) self.view.window;
		if (!win) return;

		[win keyUp: event];
	}

	- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
	{
		NativeWindow* win = (NativeWindow*) self.view.window;
		if (!win) return;

		[win touchesBegan: touches withEvent: event];
	}

	- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
	{
		NativeWindow* win = (NativeWindow*) self.view.window;
		if (!win) return;

		[win touchesEnded: touches withEvent: event];
	}

	- (void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event
	{
		NativeWindow* win = (NativeWindow*) self.view.window;
		if (!win) return;

		[win touchesCancelled: touches withEvent: event];
	}

	- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
	{
		NativeWindow* win = (NativeWindow*) self.view.window;
		if (!win) return;

		[win touchesMoved: touches withEvent: event];
	}

@end// OpenGLViewController
