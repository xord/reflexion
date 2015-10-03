// -*- objc -*-
#import "app_delegate.h"


#include <assert.h>
#include "reflex/event.h"
#include "reflex/exception.h"
#include "application_data.h"


#define REF (*pref)


@implementation AppDelegate

	{
		Reflex::Application::Ref* pref;
	}

	- (id) init
	{
		self = [super init];
		if (!self) return nil;

		pref = new Reflex::Application::Ref;

		return self;
	}

	- (void) dealloc
	{
		assert(pref && !REF);

		delete pref;

		[super dealloc];
	}

	- (void) bind: (Reflex::Application*) instance
	{
		assert(pref);

		if (instance && instance->self->delegate)
			Reflex::argument_error(__FILE__, __LINE__);

		if (REF)
			Reflex::invalid_state_error(__FILE__, __LINE__);

		REF = instance;
		if (REF) REF->self->delegate = [self retain];
	}

	- (void) unbind: (Reflex::Application*) instance
	{
		assert(pref);

		if (!REF) return;

		if (instance && instance != REF.get())
			Reflex::invalid_state_error(__FILE__, __LINE__);

		if (REF->self->delegate) [REF->self->delegate release];
		REF->self->delegate = nil;
		REF.reset();
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

		if (REF)
		{
			Reflex::Event e;
			REF->on_start(&e);
			if (e.is_blocked())
			{
				return NO;
			}
		}

		return YES;
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
		if (REF)
		{
			Reflex::Event e;
			REF->on_quit(&e);
			if (e.is_blocked())
			{
				Reflex::not_implemented_error(
					__FILE__, __LINE__, "blocking to quit is not supported.");
			}
		}

		[self unbind: NULL];
	}

@end// AppDelegate
