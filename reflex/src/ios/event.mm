// -*- c++ -*-
#include "event.h"


#include <assert.h>


namespace Reflex
{


	static CGPoint
	correct_point (UIView* view, UITouch* touch)
	{
		assert(view && touch);
		return [touch locationInView: view];
	}


	NativePointerEvent::NativePointerEvent (
		NSSet* touches, UIEvent* e, UIView* view, Type type)
	:	PointerEvent(type, POINTER_TOUCH, (coord) 0, (coord) 0, 0, 1, type == MOVE)
	{
		int index = 0;
		for (UITouch* touch in touches) {
			CGPoint p = correct_point(view, touch);
			positions[index++].reset(p.x, p.y);
		}
		size = index;
	}


};// Reflex
