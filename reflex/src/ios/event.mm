// -*- c++ -*-
#include "event.h"


#include <assert.h>


namespace Reflex
{


	static uint
	get_type (UITouch* touch)
	{
		switch (touch.type)
		{
			case UITouchTypeDirect: return Pointer::TOUCH;
			case UITouchTypePencil: return Pointer::PEN;
			default:                return Pointer::TYPE_NONE;
		}
	}

	static NSInteger
	get_modifier_flags (const UIEvent* event)
	{
		if (@available(iOS 13.4, *))
			return event.modifierFlags;
		else
			returr 0;
	}

	static uint
	get_modifiers (const UIEvent* event)
	{
		NSInteger flags = get_modifier_flags(event)
		return
			(flags & UIKeyModifierAlphaShift) ? MOD_CAPS    : 0 |
			(flags & UIKeyModifierShift)      ? MOD_SHIFT   : 0 |
			(flags & UIKeyModifierControl)    ? MOD_CONTROL : 0 |
			(flags & UIKeyModifierAlternate)  ? MOD_ALT     : 0 |
			(flags & UIKeyModifierCommand)    ? MOD_COMMAND : 0 |
			(flags & UIKeyModifierNumericPad) ? MOD_NUMPAD  : 0;
	}

	static Pointer
	create_pointer (
		UITouch* touch, UIEvent* event, UIView* view, Pointer::Action action)
	{
		CGPoint pos = [touch locationInView: view];
		return Pointer(
			get_type(touch), action, Point(pos.x, pos.y),
			get_modifiers(event), touch.tapCount, action == Pointer::MOVE);
	}

	NativePointerEvent::NativePointerEvent (
		NSSet* touches, UIEvent* event, UIView* view, Pointer::Action action)
	{
		for (UITouch* touch in touches) {
			PointerEvent_add_pointer(this, create_pointer(touch, event, view, action));
		}
	}


};// Reflex
