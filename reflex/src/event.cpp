#include "event.h"


#include "reflex/timer.h"
#include "reflex/shape.h"
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

	FocusEvent::FocusEvent (Type type, View* current, View* last)
	:	type(type), current(current), last(last)
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


	PointerEvent::Pointer::Data::Data ()
	:	action(ACTION_NONE), pointer_type(0), position(0),
		modifiers(0), click_count(0), drag(false)
	{
	}


	PointerEvent::Pointer::Pointer ()
	{
	}

	PointerEvent::Pointer::~Pointer ()
	{
	}

	PointerEvent::Action
	PointerEvent::Pointer::action () const
	{
		return self->action;
	}

	uint
	PointerEvent::Pointer::pointer_type () const
	{
		return self->pointer_type;
	}

	Point
	PointerEvent::Pointer::position () const
	{
		return self->position;
	}

	uint
	PointerEvent::Pointer::modifiers () const
	{
		return self->modifiers;
	}

	uint
	PointerEvent::Pointer::click_count () const
	{
		return self->click_count;
	}

	bool
	PointerEvent::Pointer::is_drag () const
	{
		return self->drag;
	}

	PointerEvent::Pointer
	PointerEvent::Pointer::next () const
	{
		return self->next;
	}

	PointerEvent::Pointer::operator bool () const
	{
		return ACTION_FIRST <= self->action && self->action <= ACTION_LAST;
	}

	bool
	PointerEvent::Pointer::operator ! () const
	{
		return !operator bool();
	}


	PointerEvent::Data::Data ()
	:	capture(false)
	{
	}


	PointerEvent::PointerEvent ()
	{
	}

	PointerEvent::~PointerEvent ()
	{
	}

	PointerEvent::Action
	PointerEvent::action () const
	{
		return self->pointers.front().action();
	}

	uint
	PointerEvent::pointer_type () const
	{
		return self->pointers.front().pointer_type();
	}

	Point
	PointerEvent::position () const
	{
		return self->pointers.front().position();
	}

	uint
	PointerEvent::modifiers () const
	{
		return self->pointers.front().modifiers();
	}

	uint
	PointerEvent::click_count () const
	{
		return self->pointers.front().click_count();
	}

	bool
	PointerEvent::is_drag () const
	{
		return self->pointers.front().is_drag();
	}

	const PointerEvent::PointerList&
	PointerEvent::pointers () const
	{
		return self->pointers;
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
	:	type(NONE), shape(NULL), view(NULL)
	{
	}

	ContactEvent::ContactEvent (Type type, Shape* shape)
	:	type(type), shape(shape), view(shape ? shape->owner() : NULL)
	{
	}


	MotionEvent::MotionEvent (const Point& gravity)
	:	gravity(gravity)
	{
	}


}// Reflex
