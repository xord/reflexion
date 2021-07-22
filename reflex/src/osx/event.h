// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_OSX_EVENT_H__
#define __REFLEX_SRC_OSX_EVENT_H__


#import <AppKit/NSEvent.h>
#include "../event.h"


namespace Reflex
{


	struct NativeKeyEvent : public KeyEvent
	{

		NativeKeyEvent (NSEvent* event, Type type);

	};// NativeKeyEvent


	struct NativeFlagKeyEvent : public KeyEvent
	{

		NativeFlagKeyEvent (NSEvent* event);

	};// NativeFlagKeyEvent


	struct NativeWheelEvent : public WheelEvent
	{

		NativeWheelEvent (NSEvent* event, NSView* view);

	};// NativeWheelEvent


	PointerEvent create_pointer_event (
		NSEvent* event, NSView* view, PointerEvent::Action action);


}// Reflex


#endif//EOH
