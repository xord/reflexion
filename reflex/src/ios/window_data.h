// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_IOS_WINDOW_DATA_H__
#define __REFLEX_SRC_IOS_WINDOW_DATA_H__


#include <map>
#include <xot/time.h>
#include <reflex/point.h>
#include <reflex/painter.h>
#include <reflex/window.h>
#include <reflex/view.h>


@class NativeWindow;


namespace Reflex
{


	typedef std::map<View::Ref, bool> CapturingViews;


	struct Window::Data
	{

		NativeWindow* native;

		String title;

		int hide_count;

		bool redraw;

		Painter painter;

		View::Ref root, focus;

		Point prev_position, prev_size;

		double prev_time_update, prev_time_draw, prev_fps;

		CapturingViews capturing_views;

		Data ()
		:	native(nil), hide_count(1), redraw(true), prev_fps(0)
		{
			prev_time_update = prev_time_draw = Xot::time();
		}

		operator bool () const
		{
			return native;
		}

		bool operator ! () const
		{
			return !operator bool();
		}

	};// Window::Data


}// Reflex


#endif//EOH
