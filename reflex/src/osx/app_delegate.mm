// -*- objc -*-
#import "app_delegate.h"


#include <assert.h>
#import <Cocoa/Cocoa.h>
#include "reflex/event.h"
#include "reflex/exception.h"
#include "application_data.h"


#define REF (*pref)


@implementation AppDelegate

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

	- (void) quit
	{
		assert(pref);

		if (REF)
			REF->quit();
		else
			[NSApp terminate: nil];
	}

	- (void) showPreference
	{
		assert(pref);

		if (REF)
		{
			Reflex::Event e;
			REF->on_preference(&e);
		}
	}

	- (void) showAbout
	{
		assert(pref);

		if (REF)
		{
			Reflex::Event e;
			REF->on_about(&e);
		}
		else
			[NSApp orderFrontStandardAboutPanel: nil];
	}

	- (void) applicationDidFinishLaunching: (NSNotification*) notification
	{
		[NSApp setActivationPolicy: NSApplicationActivationPolicyRegular];
		[NSApp activateIgnoringOtherApps: YES];

		if (REF)
		{
			Reflex::Event e;
			REF->on_start(&e);
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
		if (REF)
		{
			Reflex::Event e;
			REF->on_quit(&e);
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
		[self unbind: NULL];
	}

	- (BOOL) setupApplicationMenu: (NSMenu*) parent
	{
		assert(pref);
		if (!REF || !parent) return NO;

		NSMenu* menu = [[[NSMenu alloc]
			initWithTitle: @"Application"]
			autorelease];
		if ([NSApp respondsToSelector: @selector(setAppleMenu:)])
			[NSApp performSelector: @selector(setAppleMenu:) withObject: menu];

		NSString* name = !REF->self->name.empty() ?
			[NSString stringWithUTF8String: REF->self->name.c_str()] : @"";
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
