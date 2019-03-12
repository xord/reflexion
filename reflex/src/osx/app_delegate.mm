// -*- objc -*-
#import "app_delegate.h"


#include <assert.h>
#import <Cocoa/Cocoa.h>
#include "reflex/event.h"
#include "reflex/exception.h"
#include "application_data.h"


@implementation ReflexAppDelegate

	{
		Reflex::Application* pinstance;
		bool started;
	}

	- (id) init
	{
		self = [super init];
		if (!self) return nil;

		pinstance = NULL;
		started   = false;

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

	- (BOOL) callOnStart
	{
		if (!pinstance || started)
			return YES;

		Reflex::Event e;
		pinstance->on_start(&e);
		started = true;

		if (e.is_blocked()) [self quit];
		return !e.is_blocked();
	}

	- (void) quit
	{
		if (pinstance)
			pinstance->quit();
		else
			[NSApp terminate: nil];
	}

	- (void) showPreference
	{
		if (!pinstance) return;

		Reflex::Event e;
		pinstance->on_preference(&e);
	}

	- (void) showAbout
	{
		if (pinstance)
		{
			Reflex::Event e;
			pinstance->on_about(&e);
		}
		else
			[NSApp orderFrontStandardAboutPanel: nil];
	}

	- (void) applicationDidFinishLaunching: (NSNotification*) notification
	{
		[NSApp setActivationPolicy: NSApplicationActivationPolicyRegular];
		[NSApp activateIgnoringOtherApps: YES];

		if (![self callOnStart])
			return;

		[self setupMenu];
	}

	- (NSApplicationTerminateReply) applicationShouldTerminate: (NSApplication*) application
	{
		if (pinstance)
		{
			Reflex::Event e;
			pinstance->on_quit(&e);
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
		if (!pinstance || !parent) return NO;

		NSMenu* menu = [[[NSMenu alloc]
			initWithTitle: @"Application"]
			autorelease];
		if ([NSApp respondsToSelector: @selector(setAppleMenu:)])
			[NSApp performSelector: @selector(setAppleMenu:) withObject: menu];

		NSString* name = !pinstance->self->name.empty() ?
			[NSString stringWithUTF8String: pinstance->self->name.c_str()] : @"";
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

@end// ReflexAppDelegate
