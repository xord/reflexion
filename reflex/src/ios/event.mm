// -*- c++ -*-
#include "event.h"


#include <assert.h>


namespace Reflex
{


	static uint
	get_type (UITouch* touch)
	{
		NSInteger type = 0;
		if (@available(iOS 9.0, *)) type = touch.type;

		switch (type)
		{
			case UITouchTypeDirect: return Pointer::TOUCH;
			case UITouchTypePencil: return Pointer::PEN;
			default:                return Pointer::TYPE_NONE;
		}
	}

	static Pointer::Action
	get_action (UITouch* touch)
	{
		switch (touch.phase)
		{
			case UITouchPhaseBegan:         return Pointer::DOWN;
			case UITouchPhaseEnded:         return Pointer::UP;
			case UITouchPhaseMoved:         return Pointer::MOVE;
			case UITouchPhaseStationary:    return Pointer::STAY;
			case UITouchPhaseCancelled:     return Pointer::CANCEL;
			//case UITouchPhaseRegionEntered: return Pointer::MOVE;
			//case UITouchPhaseRegionExited:  return Pointer::MOVE;
			//case UITouchPhaseRegionMoved:   return Pointer::MOVE;
			default:                        return Pointer::ACTION_NONE;
		}
	}

	static uint
	get_modifiers (const UIEvent* event)
	{
		NSInteger flags = 0;
		if (@available(iOS 13.4, *)) flags = event.modifierFlags;

		return
			(flags & UIKeyModifierAlphaShift) ? MOD_CAPS    : 0 |
			(flags & UIKeyModifierShift)      ? MOD_SHIFT   : 0 |
			(flags & UIKeyModifierControl)    ? MOD_CONTROL : 0 |
			(flags & UIKeyModifierAlternate)  ? MOD_ALT     : 0 |
			(flags & UIKeyModifierCommand)    ? MOD_COMMAND : 0 |
			(flags & UIKeyModifierNumericPad) ? MOD_NUMPAD  : 0;
	}

	static Pointer
	create_pointer (UITouch* touch, UIEvent* event, UIView* view)
	{
		Pointer::Action action = get_action(touch);
		CGPoint pos            = [touch locationInView: view];
		return Pointer(
			get_type(touch), action, Point(pos.x, pos.y),
			get_modifiers(event), (uint) touch.tapCount, action == Pointer::MOVE);
	}

	NativePointerEvent::NativePointerEvent (
		NSSet* touches, UIEvent* event, UIView* view)
	{
		for (UITouch* touch in touches) {
			PointerEvent_add_pointer(this, create_pointer(touch, event, view));
		}
	}


};// Reflex
