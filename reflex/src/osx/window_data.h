// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_OSX_WINDOW_DATA_H__
#define __REFLEX_SRC_OSX_WINDOW_DATA_H__


#include <map>
#include <xot/time.h>
#include <rays/point.h>
#include <rays/painter.h>
#include <reflex/window.h>
#include <reflex/view.h>


@class NativeWindow;


namespace Reflex
{


	typedef std::map<View::Ref, bool> CapturingViews;


	struct Window::Data
	{

		NativeWindow* native = nil;

		int hide_count = 1;

		bool redraw = true;

		Painter painter;

		View::Ref root, focus;

		Point prev_position, prev_size;

		double prev_time_update = 0, prev_time_draw = 0, prev_fps = 0;

		String title_tmp;

		CapturingViews capturing_views;

		Data ()
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
