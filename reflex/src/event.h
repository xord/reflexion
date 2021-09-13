// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_EVENT_H__
#define __REFLEX_SRC_EVENT_H__


#include <reflex/event.h>


namespace Reflex
{


	void PointerEvent_update_for_capturing_views (
		PointerEvent* pthis, const View* view);

	void PointerEvent_filter_and_update_positions (
		PointerEvent* pthis, const Bounds& frame);

	void PointerEvent_scroll_and_zoom_positions (
		PointerEvent* pthis, const Point* scroll, float zoom);


}// Reflex


#endif//EOH
