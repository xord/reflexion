// -*- objc -*-
#import "app_delegate.h"


#include <assert.h>
#import <Cocoa/Cocoa.h>
#include "reflex/event.h"
#include "reflex/exception.h"
#include "application_data.h"


@implementation AppDelegate

	- (id) init
	{
		self = [super init];
		if (!self) return nil;

		pinstance = ptr_for_rebind = NULL;

		return self;
	}

	- (void) dealloc
	{
		assert(!pinstance);

		[super dealloc];
	}

	- (void) bind: (Reflex::Application*) instance
	{
		if (!instance)
			Reflex::argument_error(__FILE__, __LINE__);

		if (instance && instance->self->delegate)
			Reflex::invalid_state_error(__FILE__, __LINE__);

		instance->self->delegate = [self retain];

		// Reflex::Application is not constructed completely,
		// so can not call ClassWrapper::retain().
		instance->Xot::template RefCountable<>::retain();

		// defer calling ClassWrapper::retain() to rebind.
		ptr_for_rebind = instance;
	}

	- (void) rebind
	{
		if (!pinstance && ptr_for_rebind)
		{
			pinstance = ptr_for_rebind;
			pinstance->retain();

			ptr_for_rebind->Xot::template RefCountable<>::release();
			ptr_for_rebind = NULL;
		}

		assert(pinstance && !ptr_for_rebind);
	}

	- (void) unbind
	{
		[self rebind];
		if (!pinstance) return;

		if (pinstance->self->delegate) [pinstance->self->delegate release];
		pinstance->self->delegate = nil;

		pinstance->release();
		pinstance = NULL;
	}

	- (Reflex::Application*) instance
	{
		[self rebind];
		return pinstance;
	}

	- (void) quit
	{
		Reflex::Application* ptr = [self instance];
		if (ptr)
			ptr->quit();
		else
			[NSApp terminate: nil];
	}

	- (void) showPreference
	{
		Reflex::Application* ptr = [self instance];
		if (ptr)
		{
			Reflex::Event e;
			ptr->on_preference(&e);
		}
	}

	- (void) showAbout
	{
		Reflex::Application* ptr = [self instance];
		if (ptr)
		{
			Reflex::Event e;
			ptr->on_about(&e);
		}
		else
			[NSApp orderFrontStandardAboutPanel: nil];
	}

	- (void) applicationDidFinishLaunching: (NSNotification*) notification
	{
		[NSApp setActivationPolicy: NSApplicationActivationPolicyRegular];
		[NSApp activateIgnoringOtherApps: YES];

		Reflex::Application* ptr = [self instance];
		if (ptr)
		{
			Reflex::Event e;
			ptr->on_start(&e);
			if (e.is_blocked())
			{
				[self quit];
				return;
			}
		}

		[self setupMenu];
	}

	- (NSApplicationTerminateReply) applicationShouldTerminate: (NSApplication*) application
	{
		Reflex::Application* ptr = [self instance];
		if (ptr)
		{
			Reflex::Event e;
			ptr->on_quit(&e);
			if (e.is_blocked()) return NSTerminateCancel;
		}

		return NSTerminateNow;
	}

	- (BOOL) applicationShouldTerminateAfterLastWindowClosed: (NSApplication*) application
	{
		return YES;
	}

	- (void) applicationWillTerminate: (NSNotification*) notification
	{
		[self unbind];
	}

	- (BOOL) setupApplicationMenu: (NSMenu*) parent
	{
		Reflex::Application* ptr = [self instance];
		if (!ptr || !parent) return NO;

		NSMenu* menu = [[[NSMenu alloc]
			initWithTitle: @"Application"]
			autorelease];
		if ([NSApp respondsToSelector: @selector(setAppleMenu:)])
			[NSApp performSelector: @selector(setAppleMenu:) withObject: menu];

		NSString* name = !ptr->self->name.empty() ?
			[NSString stringWithUTF8String: ptr->self->name.c_str()] : @"";
		if ([name length] > 0)
			name = [@" " stringByAppendingString: name];

		NSMenu* submenu = nil;
		NSMenuItem* item = nil;

		[menu
			addItemWithTitle: [@"About" stringByAppendingString: name]
			action: @selector(showAbout)
			keyEquivalent: @""];

		[menu addItem: [NSMenuItem separatorItem]];

		[menu
			addItemWithTitle: @"Preferences"
			action: @selector(showPreference)
			keyEquivalent: @","];

		[menu addItem: [NSMenuItem separatorItem]];

		item = [menu
			addItemWithTitle: @"Services"
			action: nil
			keyEquivalent: @""];
		submenu = [[[NSMenu alloc] initWithTitle: @"Services"] autorelease];
		[item setSubmenu: submenu];
		[NSApp setServicesMenu: submenu];

		[menu addItem: [NSMenuItem separatorItem]];

		[menu
			addItemWithTitle: [@"Hide" stringByAppendingString: name]
			action: @selector(hide:)
			keyEquivalent: @"h"];

		item = [menu
			addItemWithTitle: @"Hide Others"
			action: @selector(hideOtherApplications:)
			keyEquivalent: @"h"];
		[item setKeyEquivalentModifierMask: NSAlternateKeyMask | NSCommandKeyMask];

		[menu
			addItemWithTitle: @"ShowAll"
			action: @selector(unhideAllApplications:)
			keyEquivalent: @""];

		[menu addItem: [NSMenuItem separatorItem]];

		[menu
			addItemWithTitle: [@"Quit" stringByAppendingString: name]
			action: @selector(quit)
			keyEquivalent: @"q"];

		item =
			[parent addItemWithTitle: @"Application" action: nil keyEquivalent: @""];
		[parent setSubmenu: menu forItem: item];

		return YES;
	}

	- (BOOL) setupWindowMenu: (NSMenu*) parent
	{
		if (!parent) return NO;

		NSMenu* menu = [[[NSMenu alloc]
			initWithTitle: @"Window"]
			autorelease];

		[menu
			addItemWithTitle: @"Minimize"
			action: @selector(performMiniaturize:)
			keyEquivalent: @"m"];

		[menu
			addItemWithTitle: @"Zoom"
			action: @selector(performZoom:)
			keyEquivalent: @""];

		[menu addItem: [NSMenuItem separatorItem]];

		[menu
			addItemWithTitle: @"Bring All to Front"
			action: @selector(arrangeInFront:)
			keyEquivalent: @""];

		NSMenuItem* item =
			[parent addItemWithTitle: @"Window" action: nil keyEquivalent: @""];
		[parent setSubmenu: menu forItem: item];

		[NSApp setWindowsMenu: menu];

		return YES;
	}

	- (BOOL) setupMenu
	{
		if ([NSApp mainMenu]) return NO;

		NSMenu* menu = [[[NSMenu alloc] initWithTitle: @"MainMenu"] autorelease];

		if (
			![self setupApplicationMenu: menu] ||
			![self setupWindowMenu: menu])
		{
			return NO;
		}

		[NSApp setMainMenu: menu];

		return YES;
	}

@end// AppDelegate
