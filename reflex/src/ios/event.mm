// -*- c++ -*-
#include "event.h"


#include <assert.h>


namespace Reflex
{


	static CGPoint
	correct_point (UIView* view, UITouch* touch)
	{
		assert(view && touch);
		CGPoint p = [touch locationInView: view];
		p.y = [view bounds].size.height - p.y;
		return p;
	}


	NativePointerEvent::NativePointerEvent (NSSet* touches, UIEvent* e, UIView* view, Type type)
	:	PointerEvent(type, POINTER_TOUCH, (coord) 0, (coord) 0, 0, 1, type == MOVE)
	{
		CGPoint p = correct_point(view, [touches anyObject]);
		x = p.x;
		y = p.y;
	}


};// Reflex
