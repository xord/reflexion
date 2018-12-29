// -*- objc -*-
#include "reflex/reflex.h"


#import <Cocoa/Cocoa.h>
#include "reflex/exception.h"
#import "app_delegate.h"


namespace Reflex
{


	namespace global
	{

		static NSAutoreleasePool* pool = nil;

	}// global


	bool
	initialized ()
	{
		return global::pool;
	}

	static Class
	get_app_delegate_class ()
	{
		id klass = NSClassFromString(@"AppDelegate");
		if (klass) return klass;

		return ReflexAppDelegate.class;
	}

	void
	init ()
	{
		if (global::pool)
			reflex_error(__FILE__, __LINE__, "already initialized.");

		global::pool = [[NSAutoreleasePool alloc] init];

		id app_delegate = [[[get_app_delegate_class() alloc] init] autorelease];
		[[NSApplication sharedApplication] setDelegate: app_delegate];
	}

	void
	fin ()
	{
		if (!global::pool)
			reflex_error(__FILE__, __LINE__, "not initialized.");

		[global::pool release];
		global::pool = nil;
	}


}// Reflex
