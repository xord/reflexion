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

	void
	init ()
	{
		if (global::pool)
			reflex_error(__FILE__, __LINE__, "already initialized.");

		global::pool = [[NSAutoreleasePool alloc] init];

		[[NSApplication sharedApplication] setDelegate: [[[AppDelegate alloc] init] autorelease]];
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
