#include "reflex/ruby/event.h"


#include <rucy.h>
#include "rays/ruby/point.h"
#include "defs.h"


using namespace Rucy;

using Reflex::coord;


RUCY_DEFINE_VALUE_FROM_TO(Reflex::WheelEvent)

#define THIS  to<Reflex::WheelEvent*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::WheelEvent, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Reflex::WheelEvent>(klass);
}
RUCY_END

static
RUCY_DEFN(initialize)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "WheelEvent#initialize", argc, 0, 1, 2, 3);

	THIS->dx = (argc >= 1) ? to<coord>(argv[0]) : 0;
	THIS->dy = (argc >= 2) ? to<coord>(argv[1]) : 0;
	THIS->dz = (argc >= 3) ? to<coord>(argv[2]) : 0;

	return rb_call_super(0, NULL);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::WheelEvent&>(obj);
	return self;
}
RUCY_END

static
RUCY_DEF0(dx)
{
	CHECK;
	return value(THIS->dx);
}
RUCY_END

static
RUCY_DEF0(dy)
{
	CHECK;
	return value(THIS->dy);
}
RUCY_END

static
RUCY_DEF0(dz)
{
	CHECK;
	return value(THIS->dz);
}
RUCY_END

static
RUCY_DEF0(x)
{
	CHECK;
	return value(THIS->x);
}
RUCY_END

static
RUCY_DEF0(y)
{
	CHECK;
	return value(THIS->y);
}
RUCY_END

static
RUCY_DEF0(z)
{
	CHECK;
	return value(THIS->z);
}
RUCY_END

static
RUCY_DEF0(modifiers)
{
	CHECK;
	return value(THIS->modifiers);
}
RUCY_END

static
RUCY_DEF0(delta)
{
	CHECK;
	return value(Rays::Point(THIS->dx, THIS->dy, THIS->dz));
}
RUCY_END

static
RUCY_DEF0(position)
{
	CHECK;
	return value(Rays::Point(THIS->x, THIS->y, THIS->z));
}
RUCY_END


static Class cWheelEvent;

void
Init_wheel_event ()
{
	Module mReflex = define_module("Reflex");

	cWheelEvent = mReflex.define_class("WheelEvent", Reflex::event_class());
	cWheelEvent.define_alloc_func(alloc);
	cWheelEvent.define_private_method("initialize",      initialize);
	cWheelEvent.define_private_method("initialize_copy", initialize_copy);
	cWheelEvent.define_method("dx", dx);
	cWheelEvent.define_method("dy", dy);
	cWheelEvent.define_method("dz", dz);
	cWheelEvent.define_method("x", x);
	cWheelEvent.define_method("y", y);
	cWheelEvent.define_method("z", z);
	cWheelEvent.define_method("modifiers", modifiers);
	cWheelEvent.define_method("delta", delta);
	cWheelEvent.define_method("position", position);
}


namespace Reflex
{


	Class
	wheel_event_class ()
	{
		return cWheelEvent;
	}


}// Reflex
