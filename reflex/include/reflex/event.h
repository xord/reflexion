// -*- c++ -*-
#pragma once
#ifndef __REFLEX_EVENT_H__
#define __REFLEX_EVENT_H__


#include <vector>
#include <xot/pimpl.h>
#include <rays/point.h>
#include <rays/bounds.h>
#include <rays/painter.h>
#include <reflex/defs.h>
#include <reflex/timer.h>


namespace Reflex
{


	class Shape;
	class View;


	class Event
	{

		public:

			Event ();

			void block ();

			bool is_blocked () const;

		private:

			bool blocked;

	};// Event


	struct UpdateEvent : public Event
	{

		double now;

		float dt;

		UpdateEvent (double now = 0, float dt = 0);

	};// UpdateEvent


	struct DrawEvent : public Event
	{

		View* view;

		Painter* painter;

		Bounds bounds;

		float dt, fps;

		DrawEvent (float dt = 0, float fps = 0);

	};// DrawEvent


	struct FrameEvent : public Event
	{

		Bounds frame;

		coord dx, dy;

		union
		{
			struct {coord dwidth, dheight;};

			struct {coord dw,     dh;};
		};

		float angle, dangle;

		FrameEvent (
			const Bounds& frame = 0, coord dx = 0, coord dy = 0, coord dwidth = 0, coord dheight = 0,
			float angle = 0, float dangle = 0);

		FrameEvent (
			const Bounds& frame, const Bounds& prev_frame,
			float angle = 0, float prev_angle = 0);

		bool is_move () const;

		bool is_resize () const;

		bool is_rotate () const;

	};// FrameEvent


	struct ScrollEvent : public Event
	{

		union
		{
			struct {coord x, y, z;};

			Coord3 scroll_;
		};

		union
		{
			struct {coord dx, dy, dz;};

			Coord3 delta_;
		};

		ScrollEvent ();

		ScrollEvent (coord x, coord y, coord z, coord dx, coord dy, coord dz);

		      Point& scroll ();

		const Point& scroll () const;

		      Point& delta ();

		const Point& delta () const;

	};// ScrollEvent


	struct FocusEvent : public Event
	{

		enum Type {NONE = 0, FOCUS, BLUR};

		Type type;

		View *current, *last;

		FocusEvent ();

		FocusEvent (Type type, View* current, View* last);

	};// FocusEvent


	struct KeyEvent : public Event
	{

		enum Type {NONE = 0, DOWN, UP};

		Type type;

		String chars;

		int code;

		uint modifiers;

		int repeat;

		bool capture;

		KeyEvent ();

		KeyEvent (
			Type type, const char* chars, int code,
			uint modifiers = 0, int repeat = 0);

	};// KeyEvent


	struct PointerEvent : public Event
	{

		enum Action
		{

			ACTION_NONE  = 0, DOWN, UP, MOVE, CANCEL, STAY,

			ACTION_FIRST = DOWN, ACTION_LAST = STAY

		};// Action

		struct Pointer
		{

			Pointer ();

			~Pointer ();

			Action action () const;

			uint pointer_type () const;

			Point position () const;

			uint modifiers () const;

			uint click_count () const;

			bool is_drag () const;

			Pointer next () const;

			operator bool () const;

			bool operator ! () const;

			struct Data;

			Xot::PImpl<Data> self;

		};// Pointer

		typedef std::vector<Pointer> PointerList;

		PointerEvent ();

		~PointerEvent ();

		Action action () const;

		uint pointer_type () const;

		Point position () const;

		uint modifiers () const;

		uint click_count () const;

		bool is_drag () const;

		const PointerList& pointers () const;

		struct Data;

		Xot::PImpl<Data> self;

	};// PointerEvent


	struct WheelEvent : public Event
	{

		union
		{
			struct {coord dx, dy, dz;};

			Coord3 delta_;
		};

		union
		{
			struct {coord x, y, z;};

			Coord3 position_;
		};

		uint modifiers;

		WheelEvent ();

		WheelEvent (
			coord dx, coord dy, coord dz, coord x = 0, coord y = 0, coord z = 0,
			uint modifiers = 0);

		      Point& position ();

		const Point& position () const;

		      Point& delta ();

		const Point& delta () const;

	};// WheelEvent


	struct CaptureEvent : public Event
	{

		uint begin, end;

		CaptureEvent ();

		CaptureEvent (uint begin, uint end);

	};// CaptureEvent


	struct TimerEvent : public Event
	{

		Timer::Ref timer;

		TimerEvent (Timer* timer = NULL);

		View* owner () const;

		int id () const;

		float interval () const;

		void set_count (int count);

		int      count () const;

		bool is_finished () const;

	};// TimerEvent


	struct ContactEvent : public Event
	{

		enum Type {NONE = 0, BEGIN, END};

		Type type;

		Shape* shape;

		View* view;

		ContactEvent ();

		ContactEvent (Type type, Shape* shape);

	};// ContactEvent


	struct MotionEvent : public Event
	{

		Point gravity;

		MotionEvent (const Point& gravity = Point(0, 9.8));

	};// MotionEvent


}// Reflex


#endif//EOH
