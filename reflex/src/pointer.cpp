#include "pointer.h"


#include <xot/time.h>
#include "reflex/exception.h"


namespace Reflex
{


	class PrevPointerPtr : public Xot::PImpl<Pointer>
	{

		typedef Xot::PImpl<Pointer> Super;

		public:

			PrevPointerPtr () : Super(NULL) {}

	};// PrevPointerPtr


	struct Pointer::Data
	{

		enum Flag
		{

			DRAG  = Xot::bit(0),

			ENTER = Xot::bit(1),

			EXIT  = Xot::bit(2),

		};// Flag

		uint type;

		Action action;

		double time;

		Point position;

		uint modifiers, click_count, flags;

		PrevPointerPtr prev;

		Data (
			uint type = TYPE_NONE, Action action = ACTION_NONE, double time = 0,
			const Point& position = 0, uint modifiers = 0, uint click_count = 0,
			bool drag = false)
		:	type(type), action(action), time(time),
			position(position), modifiers(modifiers), click_count(click_count),
			flags(make_flags(drag, false, false))
		{
		}

		uint make_flags (bool drag, bool enter, bool exit)
		{
			return
				(drag  ? DRAG  : 0) |
				(enter ? ENTER : 0) |
				(exit  ? EXIT  : 0);
		}

	};// Pointer::Data


	void
	Pointer_update_positions (Pointer* pthis, std::function<Point(const Point&)> fun)
	{
		auto& self = pthis->self;
		self->position = fun(self->position);
		if (self->prev)
			self->prev->self->position = fun(self->prev->self->position);
	}

	void
	Pointer_set_prev (Pointer* pthis, const Pointer& prev)
	{
		pthis->self->prev.reset(new Pointer(prev));
	}


	Pointer::Pointer ()
	{
	}

	Pointer::Pointer (
		uint type, Action action, double time,
		const Point& position, uint modifiers, uint click_count,
		bool drag)
	:	self(new Data(type, action, time, position, modifiers, click_count, drag))
	{
	}

	Pointer::Pointer (const This& obj)
	:	self(new Data(*obj.self))
	{
	}

	Pointer&
	Pointer::operator = (const This& obj)
	{
		if (&obj == this) return *this;

		*self = *obj.self;
		return *this;
	}

	Pointer::~Pointer ()
	{
	}

	uint
	Pointer::type () const
	{
		return self->type;
	}

	Pointer::Action
	Pointer::action () const
	{
		return self->action;
	}

	double
	Pointer::time () const
	{
		return self->time;
	}

	const Point&
	Pointer::position () const
	{
		return self->position;
	}

	uint
	Pointer::modifiers () const
	{
		return self->modifiers;
	}

	uint
	Pointer::click_count () const
	{
		return self->click_count;
	}

	bool
	Pointer::is_drag () const
	{
		return self->flags & Data::DRAG;
	}

	const Pointer*
	Pointer::prev () const
	{
		return self->prev.get();
	}

	Pointer::operator bool () const
	{
		return
			self->type != TYPE_NONE &&
			ACTION_NONE < self->action && self->action <= STAY;
	}

	bool
	Pointer::operator ! () const
	{
		return !operator bool();
	}


}// Reflex
