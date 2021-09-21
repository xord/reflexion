// -*- c++ -*-
#include "event.h"


#include <assert.h>
#include <algorithm>
#include "../pointer.h"


namespace Reflex
{


	static uint
	get_type (UITouch* touch)
	{
		assert(touch);

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
		assert(touch);

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

	static Point
	to_point (const CGPoint& point)
	{
		return Point(point.x, point.y);
	}

	static uint
	get_modifiers (const UIEvent* event)
	{
		assert(event);

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

	static void
	attach_prev_pointer (
		Pointer* pointer, PrevPointerList* prev_pointers, const Point& position)
	{
		assert(pointer && prev_pointers);

		auto it = std::find_if(
			prev_pointers->begin(), prev_pointers->end(),
			[&](const Reflex::Pointer& p) {return p.position() == position;});
		if (it == prev_pointers->end()) return;

		Reflex::Pointer_set_prev(pointer, *it);
		prev_pointers->erase(it);
	}

	static Pointer
	create_pointer (
		UITouch* touch, UIEvent* event, UIView* view, double time,
		PrevPointerList* prev_pointers)
	{
		Reflex::Pointer::Action action = get_action(touch);
		Reflex::Pointer pointer(
			get_type(touch),
			action,
			time,
			to_point([touch locationInView: view]),
			get_modifiers(event),
			(uint) touch.tapCount,
			action == Pointer::MOVE);

		if (prev_pointers)
		{
			attach_prev_pointer(
				&pointer, prev_pointers,
				to_point([touch previousLocationInView: view]));
		}

		return pointer;
	}

	NativePointerEvent::NativePointerEvent (
		NSSet* touches, UIEvent* event, UIView* view,
		PrevPointerList* prev_pointers)
	{
		for (UITouch* touch in touches) {
			PointerEvent_add_pointer(
				this, create_pointer(touch, event, view, time(), prev_pointers));
		}
	}


};// Reflex
