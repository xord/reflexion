#include "reflex/event.h"


#include "reflex/timer.h"
#include "reflex/exception.h"


namespace Reflex
{


	Event::Event ()
	:	blocked(false)
	{
	}

	void
	Event::block ()
	{
		blocked = true;
	}

	bool
	Event::is_blocked () const
	{
		return blocked;
	}


	MotionEvent::MotionEvent (const Point& gravity)
	:	gravity(gravity)
	{
	}


	UpdateEvent::UpdateEvent (double now, float dt)
	:	now(now), dt(dt)
	{
	}


	DrawEvent::DrawEvent (float dt, float fps)
	:	view(NULL), painter(NULL), dt(dt), fps(fps)
	{
	}


	FrameEvent::FrameEvent (
		const Bounds& frame, coord dx, coord dy, coord dwidth, coord dheight,
		float angle, float dangle)
	:	frame(frame), dx(dx), dy(dy), dwidth(dwidth), dheight(dheight),
		angle(angle), dangle(dangle)
	{
	}

	FrameEvent::FrameEvent (
		const Bounds& frame, const Bounds& prev_frame,
		float angle, float prev_angle)
	:	frame(frame),
		dx(    frame.x - prev_frame.x), dy(     frame.y - prev_frame.y),
		dwidth(frame.w - prev_frame.w), dheight(frame.h - prev_frame.h),
		angle(angle), dangle(angle - prev_angle)
	{
	}

	bool
	FrameEvent::is_move () const
	{
		return dx != 0 || dy != 0;
	}

	bool
	FrameEvent::is_resize () const
	{
		return dwidth != 0 || dheight != 0;
	}

	bool
	FrameEvent::is_rotate () const
	{
		return dangle != 0;
	}


	ScrollEvent::ScrollEvent ()
	:	x(0), y(0), z(0), dx(0), dy(0), dz(0)
	{
	}

	ScrollEvent::ScrollEvent (coord x, coord y, coord z, coord dx, coord dy, coord dz)
	:	x(x), y(y), z(z), dx(dx), dy(dy), dz(dz)
	{
	}

	Point&
	ScrollEvent::scroll ()
	{
		return *(Point*) &scroll_;
	}

	const Point&
	ScrollEvent::scroll () const
	{
		return const_cast<ScrollEvent*>(this)->scroll();
	}

	Point&
	ScrollEvent::delta ()
	{
		return *(Point*) &delta_;
	}

	const Point&
	ScrollEvent::delta () const
	{
		return const_cast<ScrollEvent*>(this)->delta();
	}


	FocusEvent::FocusEvent ()
	{
	}

	FocusEvent::FocusEvent (Type type, View* focus, View* last)
	:	type(type), focus(focus), last(last)
	{
	}


	KeyEvent::KeyEvent ()
	:	type(NONE), code(KEY_NONE), modifiers(MOD_NONE), repeat(0), capture(false)
	{
	}

	KeyEvent::KeyEvent (
		Type type, const char* chars, int code, uint modifiers, int repeat)
	:	type(type), chars(chars ? chars : ""), code(code), modifiers(modifiers),
		repeat(repeat), capture(false)
	{
	}


	PointerEvent::PointerEvent ()
	:	type(NONE), pointer_type(POINTER_NONE),
		size(0), modifiers(MOD_NONE), count(0), drag(false), capture(false),
		x(0), y(0), z(0)
	{
	}

	PointerEvent::PointerEvent (
		Type type, uint pointer_type, coord x, coord y, uint modifiers, uint count, bool drag)
	:	type(type), pointer_type(pointer_type),
		size(1), modifiers(modifiers), count(count), drag(drag), capture(false),
		x(x), y(y), z(0)
	{
	}

	PointerEvent::PointerEvent (
		Type type, uint pointer_type, const Point* positions_, size_t size,
		uint modifiers, uint count, bool drag)
	:	type(type), pointer_type(pointer_type),
		size(size), modifiers(modifiers), count(count), drag(drag), capture(false)
	{
		if (!positions_ && size > 0)
			argument_error(__FILE__, __LINE__);

		if (size > MAX) size = MAX;

		if (positions_)
		{
			for (size_t i = 0; i < size; ++i)
				positions[i] = *(Coord3*) &positions_[i];
		}
	}

	Point&
	PointerEvent::position (size_t i)
	{
		if (i >= size)
			index_error(__FILE__, __LINE__);

		return *(Point*) &positions[i];
	}

	const Point&
	PointerEvent::position (size_t i) const
	{
		return const_cast<PointerEvent*>(this)->position(i);
	}

	Point&
	PointerEvent::operator [] (size_t i)
	{
		return position(i);
	}

	const Point&
	PointerEvent::operator [] (size_t i) const
	{
		return position(i);
	}


	WheelEvent::WheelEvent ()
	:	dx(0), dy(0), dz(0), x(0), y(0), z(0), modifiers(0)
	{
	}

	WheelEvent::WheelEvent (
		coord dx, coord dy, coord dz, coord x, coord y, coord z, uint modifiers)
	:	dx(dx), dy(dy), dz(dz), x(x), y(y), z(z), modifiers(modifiers)
	{
	}

	Point&
	WheelEvent::position ()
	{
		return *(Point*) &position_;
	}

	const Point&
	WheelEvent::position () const
	{
		return const_cast<WheelEvent*>(this)->position();
	}

	Point&
	WheelEvent::delta ()
	{
		return *(Point*) &delta_;
	}

	const Point&
	WheelEvent::delta () const
	{
		return const_cast<WheelEvent*>(this)->delta();
	}


	CaptureEvent::CaptureEvent ()
	:	begin(0), end(0)
	{
	}

	CaptureEvent::CaptureEvent (uint begin, uint end)
	:	begin(begin), end(end)
	{
	}


	TimerEvent::TimerEvent (Timer* timer)
	:	timer(timer)
	{
	}

	View*
	TimerEvent::owner () const
	{
		return timer ? timer->owner() : NULL;
	}

	int
	TimerEvent::id () const
	{
		return timer ? timer->id() : Timer::ID_INVALID;
	}

	float
	TimerEvent::interval () const
	{
		return timer ? timer->interval() : -1;
	}

	void
	TimerEvent::set_count (int count)
	{
		if (timer) timer->set_count(count);
	}

	int
	TimerEvent::count () const
	{
		return timer ? timer->count() : 0;
	}

	bool
	TimerEvent::is_finished () const
	{
		return timer ? timer->is_finished() : true;
	}


	ContactEvent::ContactEvent ()
	:	type(NONE), view(NULL)
	{
	}

	ContactEvent::ContactEvent (Type type, View* view)
	:	type(type), view(view)
	{
	}


}// Reflex
