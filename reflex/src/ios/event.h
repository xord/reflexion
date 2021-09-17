// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_IOS_EVENT_H__
#define __REFLEX_SRC_IOS_EVENT_H__


#import <UIKit/UIEvent.h>
#include "../event.h"


namespace Reflex
{


	struct NativePointerEvent : public PointerEvent
	{

		NativePointerEvent (NSSet* touches, UIEvent* event, UIView* view);

	};// NativePointerEvent


}// Reflex


#endif//EOH
