// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_OSX_EVENT_H__
#define __REFLEX_SRC_OSX_EVENT_H__


#import <AppKit/NSEvent.h>
#include <reflex/event.h>


namespace Reflex
{


	struct NativeKeyEvent : public KeyEvent
	{

		NativeKeyEvent (NSEvent* event, Type type);

	};// NativeKeyEvent


	struct NativePointerEvent : public PointerEvent
	{

		NativePointerEvent (NSEvent* event, NSView* view, Type type);

	};// NativePointerEvent


	struct NativeWheelEvent : public WheelEvent
	{

		NativeWheelEvent (NSEvent* event, NSView* view);

	};// NativeWheelEvent


}// Reflex


#endif//EOH
