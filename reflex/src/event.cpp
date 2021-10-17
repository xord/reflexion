#include "event.h"


#include "reflex/timer.h"
#include "reflex/shape.h"
#include "reflex/exception.h"
#include "view.h"
#include "pointer.h"


namespace Reflex
{


	Event::Event ()
	:	blocked(false), time_(Xot::time())
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

	double
	Event::time () const
	{
		return time_;
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
	:	type(NONE), code(KEY_NONE), modifiers(MOD_NONE), repeat(0), captured(false)
	{
	}

	KeyEvent::KeyEvent (
		Type type, const char* chars, int code, uint modifiers, int repeat)
	:	type(type), chars(chars ? chars : ""), code(code), modifiers(modifiers),
		repeat(repeat), captured(false)
	{
	}


	struct PointerEvent::Data
	{

		std::vector<Pointer> pointers;

		bool captured;

		Data (bool captured = false)
		:	captured(captured)
		{
		}

	};// PointerEvent::Data


	void
	PointerEvent_add_pointer (PointerEvent* pthis, const Pointer& pointer)
	{
		if (!pthis)
			argument_error(__FILE__, __LINE__);

		pthis->self->pointers.emplace_back(pointer);
	}

	void
	PointerEvent_erase_pointer (PointerEvent* pthis, Pointer::ID id)
	{
		if (!pthis)
			argument_error(__FILE__, __LINE__);

		auto& pointers = pthis->self->pointers;
		auto it = std::find_if(
			pointers.begin(), pointers.end(),
			[=](const auto& pointer)
			{
				return pointer.id() == id;
			});

		if (it == pointers.end()) return;

		pointers.erase(it);
	}

	Pointer&
	PointerEvent_pointer_at (PointerEvent* pthis, size_t index)
	{
		if (!pthis)
			argument_error(__FILE__, __LINE__);

		auto& pointers = pthis->self->pointers;
		if (index >= pointers.size())
			index_error(__FILE__, __LINE__);

		return pointers[index];
	}

	void
	PointerEvent_each_pointer (
		const PointerEvent* pthis, std::function<void(const Pointer&)> fun)
	{
		if (!pthis)
			argument_error(__FILE__, __LINE__);

		for (const auto& pointer : pthis->self->pointers)
			fun(pointer);
	}

	static void
	filter_and_offset_pointer_positions (PointerEvent* event, const Bounds& frame)
	{
		assert(event);

		const Point& offset = frame.position();

		std::vector<Pointer> pointers;
		for (const auto& pointer : event->self->pointers)
		{
			if (!frame.is_include(pointer.position()))
				continue;

			pointers.emplace_back(pointer);
			Pointer_update_positions(&pointers.back(), [&](Point* pos)
			{
				*pos -= offset;
			});
		}

		event->self->pointers = pointers;
	}

	static void
	scroll_and_zoom_pointer_positions (
		PointerEvent* event, const Point& scroll, float zoom)
	{
		assert(event);

		if (zoom == 0)
			argument_error(__FILE__, __LINE__);

		if (scroll == 0 && zoom == 1)
			return;

		for (auto& pointer : event->self->pointers)
		{
			Pointer_update_positions(&pointer, [=](Point* pos)
			{
				*pos -= scroll;
				*pos /= zoom;
			});
		}
	}

	void
	PointerEvent_update_for_child_view (PointerEvent* pthis, const View* view)
	{
		if (!pthis || !view)
			argument_error(__FILE__, __LINE__);

		filter_and_offset_pointer_positions(pthis, view->frame());
		scroll_and_zoom_pointer_positions(pthis, view->scroll(), view->zoom());
	}

	void
	PointerEvent_update_for_capturing_view (PointerEvent* pthis, const View* view)
	{
		if (!pthis || !view)
			argument_error(__FILE__, __LINE__);

		for (auto& pointer : pthis->self->pointers)
		{
			Pointer_update_positions(&pointer, [=](Point* pos)
			{
				*pos = view->from_window(*pos);
			});
		}

		scroll_and_zoom_pointer_positions(pthis, view->scroll(), view->zoom());
	}


	PointerEvent::PointerEvent (bool captured)
	:	self(new Data(captured))
	{
	}

	PointerEvent::PointerEvent (const Pointer& pointer, bool captured)
	:	self(new Data(captured))
	{
		self->pointers.emplace_back(pointer);
	}

	PointerEvent::PointerEvent (const Pointer* pointers, size_t size, bool captured)
	:	self(new Data(captured))
	{
		for (size_t i = 0; i < size; ++i)
			self->pointers.emplace_back(pointers[i]);
	}

	PointerEvent::PointerEvent (const This& obj)
	:	self(new Data(*obj.self))
	{
	}

	PointerEvent&
	PointerEvent::operator = (const This& obj)
	{
		if (&obj == this) return *this;

		Event::operator=(obj);
		*self = *obj.self;
		return *this;
	}

	PointerEvent::~PointerEvent ()
	{
	}

	size_t
	PointerEvent::size () const
	{
		return self->pointers.size();
	}

	bool
	PointerEvent::empty () const
	{
		return size() == 0;
	}

	bool
	PointerEvent::is_captured () const
	{
		return self->captured;
	}

	const Pointer&
	PointerEvent::operator [] (size_t index) const
	{
		if (index >= self->pointers.size())
			index_error(__FILE__, __LINE__);

		return self->pointers[index];
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
