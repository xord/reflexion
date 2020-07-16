#include "reflex/ruby/event.h"


#include "rays/ruby/point.h"
#include "defs.h"


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
RUCY_DEF3(initialize, dx, dy, dz)
{
	CHECK;

	THIS->dx = to<coord>(dx);
	THIS->dy = to<coord>(dy);
	THIS->dz = to<coord>(dz);

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
}


namespace Reflex
{


	Class
	wheel_event_class ()
	{
		return cWheelEvent;
	}


}// Reflex
