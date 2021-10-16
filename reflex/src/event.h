// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_EVENT_H__
#define __REFLEX_SRC_EVENT_H__


#include <functional>
#include <reflex/event.h>


namespace Reflex
{


	void PointerEvent_add_pointer    (PointerEvent* pthis, const Pointer& pointer);

	void PointerEvent_erase_pointer  (PointerEvent* pthis, Pointer::ID id);

	Pointer& PointerEvent_pointer_at (PointerEvent* pthis, size_t index);

	void PointerEvent_each_pointer (
		const PointerEvent* pthis, std::function<void(const Pointer&)> fun);

	void PointerEvent_update_positions_for_capturing_views (
		PointerEvent* pthis, const View* view);

	void PointerEvent_filter_and_update_positions (
		PointerEvent* pthis, const Bounds& frame);

	void PointerEvent_scroll_and_zoom_positions (
		PointerEvent* pthis, const Point* scroll, float zoom);


}// Reflex


#endif//EOH
