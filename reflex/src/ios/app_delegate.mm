// -*- objc -*-
#import "app_delegate.h"


#include <assert.h>
#import  <CoreMotion/CoreMotion.h>
#include "reflex/application.h"
#include "reflex/event.h"
#include "reflex/exception.h"
#include "application_data.h"


@implementation ReflexAppDelegate

	{
		Reflex::Application *pinstance;
		CMMotionManager* motion_manager;
	}

	- (id) init
	{
		self = [super init];
		if (!self) return nil;

		pinstance = NULL;

		return self;
	}

	- (void) dealloc
	{
		assert(!pinstance);

		[self stopUpdateDeviceMotion];

		[super dealloc];
	}

	- (void) bind: (Reflex::Application*) instance
	{
		if (!instance)
			Reflex::argument_error(__FILE__, __LINE__);

		if (instance && instance->self->delegate)
			Reflex::invalid_state_error(__FILE__, __LINE__);

		instance->self->delegate = [self retain];
		instance->retain();

		pinstance = instance;
	}

	- (void) unbind
	{
		if (!pinstance) return;

		if (pinstance->self->delegate) [pinstance->self->delegate release];
		pinstance->self->delegate = nil;

		pinstance->release();
		pinstance = NULL;
	}

	- (Reflex::Application*) instance
	{
		return pinstance;
	}

	- (BOOL) application: (UIApplication*) application
		didFinishLaunchingWithOptions: (NSDictionary*) options
	{
		if (!Reflex::app())
		{
			Reflex::invalid_state_error(
				__FILE__, __LINE__,
				"no Application instance on application:didFinishLaunchingWithOptions:");
		}

		[self bind: Reflex::app()];

		[self startUpdateDeviceMotion];

		Reflex::Application* ptr = [self instance];
		if (ptr)
		{
			Reflex::Event e;
			ptr->on_start(&e);
			if (e.is_blocked())
			{
				return NO;
			}
		}

		return YES;
	}

	- (void) startUpdateDeviceMotion
	{
		CMMotionManager* man = [[[CMMotionManager alloc] init] autorelease];
		if (!man || !man.deviceMotionAvailable)
			return;

		man.deviceMotionUpdateInterval = 1 / 60.0;
		[man
			startDeviceMotionUpdatesToQueue: NSOperationQueue.currentQueue
			withHandler: ^(CMDeviceMotion* motion, NSError* error)
			{
				Reflex::Application* ptr = instance;
				if (!ptr) return;

				Reflex::MotionEvent e(
					Reflex::Point(motion.gravity.x, -motion.gravity.y, motion.gravity.z));
				ptr->on_motion(&e);
			}
		];

		motion_manager = [man retain];
	}

	- (void) stopUpdateDeviceMotion
	{
		if (!motion_manager || !motion_manager.deviceMotionActive)
			return;

		[motion_manager stopDeviceMotionUpdates];
		[motion_manager release];
		motion_manager = nil;
	}

	- (void) applicationWillResignActive: (UIApplication*) application
	{
	}

	- (void) applicationDidEnterBackground: (UIApplication*) application
	{
	}

	- (void) applicationWillEnterForeground: (UIApplication*) application
	{
	}

	- (void) applicationDidBecomeActive: (UIApplication*) application
	{
	}

	- (void) applicationWillTerminate: (UIApplication*) application
	{
		Reflex::Application* ptr = [self instance];
		if (ptr)
		{
			Reflex::Event e;
			ptr->on_quit(&e);
			if (e.is_blocked())
			{
				Reflex::not_implemented_error(
					__FILE__, __LINE__, "blocking to quit is not supported.");
			}
		}

		[self unbind];
	}

@end// ReflexAppDelegate
