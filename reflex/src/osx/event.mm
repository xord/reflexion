// -*- c++ -*-
#include "event.h"


#include <assert.h>
#import <Cocoa/Cocoa.h>


namespace Reflex
{


	static const char*
	get_chars (NSEvent* e)
	{
		NSString* chars = [e characters];//charactersIgnoringModifiers];
		return [chars UTF8String];
	}

	static uint
	get_current_pointer_type ()
	{
		NSUInteger buttons = [NSEvent pressedMouseButtons];
		uint ret = 0;
		if (buttons &  (1 << 0)) ret |= POINTER_MOUSE_LEFT;
		if (buttons &  (1 << 1)) ret |= POINTER_MOUSE_RIGHT;
		if (buttons >= (1 << 2)) ret |= POINTER_MOUSE_MIDDLE;
		return ret;
	}

	static uint
	get_pointer_type (NSEvent* e)
	{
		switch ([e type])
		{
			case NSLeftMouseDown:
			case NSLeftMouseUp:
			case NSLeftMouseDragged:
				return POINTER_MOUSE_LEFT;

			case NSRightMouseDown:
			case NSRightMouseUp:
			case NSRightMouseDragged:
				return POINTER_MOUSE_RIGHT;

			case NSOtherMouseDown:
			case NSOtherMouseUp:
			case NSOtherMouseDragged:
				return POINTER_MOUSE_MIDDLE;

			case NSMouseMoved:
				return get_current_pointer_type();
		}
		return 0;
	}

	static uint
	get_modifiers (const NSEvent* e)
	{
		NSUInteger flags = [e modifierFlags];
		return
			(flags & NSAlphaShiftKeyMask) ? MOD_CAPS     : 0 |
			(flags & NSShiftKeyMask)      ? MOD_SHIFT    : 0 |
			(flags & NSControlKeyMask)    ? MOD_CONTROL  : 0 |
			(flags & NSAlternateKeyMask)  ? MOD_ALT      : 0 |
			(flags & NSCommandKeyMask)    ? MOD_COMMAND  : 0 |
			(flags & NSNumericPadKeyMask) ? MOD_NUMPAD   : 0 |
			(flags & NSHelpKeyMask)       ? MOD_HELP     : 0 |
			(flags & NSFunctionKeyMask)   ? MOD_FUNCTION : 0;
	}

	static NSPoint
	correct_point (NSView* view, const NSPoint& point)
	{
		assert(view);
		NSPoint p = [view convertPoint: point fromView: nil];
		p.y = [view bounds].size.height - p.y;
		return p;
	}


	NativeKeyEvent::NativeKeyEvent (NSEvent* e, Type type)
	:	KeyEvent(
			type, get_chars(e), [e keyCode], get_modifiers(e), [e isARepeat] ? 1 : 0)
	{
	}


	NativePointerEvent::NativePointerEvent (NSEvent* e, NSView* view, Type type)
	:	PointerEvent(
			type, get_pointer_type(e), (coord) 0, (coord) 0,
			get_modifiers(e), (uint) [e clickCount],
			[e type] == NSLeftMouseDragged || [e type] == NSRightMouseDragged || [e type] == NSOtherMouseDragged)
	{
		NSPoint p = correct_point(view, [e locationInWindow]);
		x = p.x;
		y = p.y;
	}


	NativeWheelEvent::NativeWheelEvent (NSEvent* e, NSView* view)
	:	WheelEvent([e deltaX], [e deltaY], [e deltaZ])
	{
		NSPoint p = correct_point(view, [e locationInWindow]);
		x         = p.x;
		y         = p.y;
		z         = 0;
		modifiers = get_modifiers(e);
	}


};// Reflex
