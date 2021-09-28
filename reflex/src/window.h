// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_WINDOW_H__
#define __REFLEX_SRC_WINDOW_H__


#include <map>
#include <xot/time.h>
#include <rays/point.h>
#include <rays/painter.h>
#include <reflex/window.h>
#include <reflex/view.h>


namespace Reflex
{


	struct Window::Data
	{

		typedef std::map<View::Ref, bool> CapturingViews;

		int hide_count = 1;

		bool redraw = true;

		Painter painter;

		View::Ref root, focus;

		Point prev_position, prev_size;

		double prev_time_update, prev_time_draw, prev_fps = 0;

		CapturingViews capturing_views;

		Data ()
		{
			prev_time_update = prev_time_draw = Xot::time();
		}

		virtual ~Data ()
		{
		}

		virtual bool is_valid () const = 0;

		operator bool () const
		{
			return is_valid();
		}

		bool operator ! () const
		{
			return !operator bool();
		}

	};// Window::Data


	Window::Data* Window_create_data ();

	void Window_initialize (Window* window);

	void Window_show (Window* window);

	void Window_hide (Window* window);

	void Window_close (Window* window);

	void        Window_set_title (      Window* window, const char* title);

	const char* Window_get_title (const Window& window);

	void   Window_set_frame (
		Window* window, coord x, coord y, coord width, coord height);

	Bounds Window_get_frame (const Window& window);

	void Window_set_resizable (Window* window, bool state);

	bool Window_is_resizable (const Window& window);

	void Window_set_focus (Window* window, View* view);

	void Window_register_capture   (Window* window, View* view);

	void Window_unregister_capture (Window* window, View* view);

	void Window_call_draw_event (Window* window, DrawEvent* event);

	float Window_get_pixel_density (const Window& window);


	typedef View* (*Window_CreateRootViewFun) ();

	void Window_set_create_root_view_fun (Window_CreateRootViewFun fun);


}// Reflex


#endif//EOH
