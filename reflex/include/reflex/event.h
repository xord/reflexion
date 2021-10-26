// -*- c++ -*-
#pragma once
#ifndef __REFLEX_EVENT_H__
#define __REFLEX_EVENT_H__


#include <xot/pimpl.h>
#include <rays/point.h>
#include <rays/bounds.h>
#include <rays/painter.h>
#include <reflex/defs.h>
#include <reflex/pointer.h>
#include <reflex/timer.h>


namespace Reflex
{


	class Shape;
	class View;


	class Event
	{

		public:

			Event ();

			~Event ();

			void block ();

			bool is_blocked () const;

			double time () const;

			struct Data;

			Xot::PSharedImpl<Data> self;

		protected:

			Event (const Event* src);

	};// Event


	class UpdateEvent : public Event
	{

		public:

			UpdateEvent (double now = 0, float dt = 0);

			~UpdateEvent ();

			UpdateEvent dup () const;

			double now () const;

			float dt () const;

			struct Data;

			Xot::PSharedImpl<Data> self;

		private:

			UpdateEvent (const UpdateEvent* src);

	};// UpdateEvent


	class DrawEvent : public Event
	{

		public:

			DrawEvent (float dt = 0, float fps = 0);

			DrawEvent dup () const;

			      Painter* painter ();

			const Painter* painter () const;

			const Bounds& bounds () const;

			float dt () const;

			float fps () const;

			struct Data;

			Xot::PSharedImpl<Data> self;

		private:

			DrawEvent (const DrawEvent* src);

	};// DrawEvent


	class FrameEvent : public Event
	{

		public:

			FrameEvent (
				const Bounds& frame = 0,
				coord dx     = 0, coord dy      = 0,
				coord dwidth = 0, coord dheight = 0,
				float angle  = 0, float dangle  = 0);

			FrameEvent (
				const Bounds& frame, const Bounds& prev_frame,
				float angle = 0, float prev_angle = 0);

			FrameEvent dup () const;

			const Bounds& frame () const;

			coord dx () const;

			coord dy () const;

			coord dwidth () const;

			coord dheight () const;

			float angle () const;

			float dangle () const;

			bool is_move () const;

			bool is_resize () const;

			bool is_rotate () const;

			struct Data;

			Xot::PSharedImpl<Data> self;

		private:

			FrameEvent (const FrameEvent* src);

	};// FrameEvent


	struct ScrollEvent : public Event
	{

		public:

			ScrollEvent ();

			ScrollEvent (coord x, coord y, coord z, coord dx, coord dy, coord dz);

			ScrollEvent dup () const;

			      Point& scroll ();

			const Point& scroll () const;

			      Point& dscroll ();

			const Point& dscroll () const;

			struct Data;

			Xot::PSharedImpl<Data> self;

		private:

			ScrollEvent (const ScrollEvent* src);

	};// ScrollEvent


	struct FocusEvent : public Event
	{

		public:

			enum Action {ACTION_NONE = 0, FOCUS, BLUR};

			FocusEvent ();

			FocusEvent (Action action, View* current, View* last);

			FocusEvent dup () const;

			Action action () const;

			View* current () const;

			View* last () const;

			struct Data;

			Xot::PSharedImpl<Data> self;

		private:

			FocusEvent (const FocusEvent* src);

	};// FocusEvent


	struct KeyEvent : public Event
	{

		public:

			enum Action {ACTION_NONE = 0, DOWN, UP};

			KeyEvent ();

			KeyEvent (
				Action action, const char* chars, int code,
				uint modifiers = 0, int repeat = 0);

			KeyEvent dup () const;

			Action action () const;

			const char* chars () const;

			int code () const;

			uint modifiers () const;

			int repeat () const;

			bool is_captured () const;

			struct Data;

			Xot::PSharedImpl<Data> self;

		private:

			KeyEvent (const KeyEvent* src);

	};// KeyEvent


	class PointerEvent : public Event
	{

		public:

			PointerEvent ();

			PointerEvent (const Pointer* pointers, size_t size);

			PointerEvent dup () const;

			size_t size () const;

			bool empty () const;

			bool is_captured () const;

			const Pointer& operator [] (size_t index) const;

			struct Data;

			Xot::PSharedImpl<Data> self;

		private:

			PointerEvent (const PointerEvent* src);

	};// PointerEvent


	struct WheelEvent : public Event
	{

		public:

			WheelEvent ();

			WheelEvent (
				coord x, coord y, coord z, coord dx, coord dy, coord dz,
				uint modifiers = 0);

			WheelEvent dup () const;

			      Point& position ();

			const Point& position () const;

			      Point& dposition ();

			const Point& dposition () const;

			uint modifiers () const;

			struct Data;

			Xot::PSharedImpl<Data> self;

		private:

			WheelEvent (const WheelEvent* src);

	};// WheelEvent


	struct CaptureEvent : public Event
	{

		public:

			CaptureEvent ();

			CaptureEvent (uint begin, uint end);

			CaptureEvent dup () const;

			uint begin () const;

			uint end () const;

			struct Data;

			Xot::PSharedImpl<Data> self;

		private:

			CaptureEvent (const CaptureEvent* src);

	};// CaptureEvent


	struct TimerEvent : public Event
	{

		public:

			TimerEvent (Timer* timer = NULL);

			TimerEvent dup () const;

			      Timer* timer ();

			const Timer* timer () const;

			View* owner () const;

			int id () const;

			float interval () const;

			int count () const;

			bool is_finished () const;

			struct Data;

			Xot::PSharedImpl<Data> self;

		private:

			TimerEvent (const TimerEvent* src);

	};// TimerEvent


	struct ContactEvent : public Event
	{

		public:

			enum Action {ACTION_NONE = 0, BEGIN, END};

			ContactEvent ();

			ContactEvent (Action action, Shape* shape);

			ContactEvent dup () const;

			Action action () const;

			      Shape* shape ();

			const Shape* shape () const;

			      View* view ();

			const View* view () const;

			struct Data;

			Xot::PSharedImpl<Data> self;

		private:

			ContactEvent (const ContactEvent* src);

	};// ContactEvent


	struct MotionEvent : public Event
	{

		Point gravity;

		MotionEvent (const Point& gravity = Point(0, 9.8));

	};// MotionEvent


}// Reflex


#endif//EOH
