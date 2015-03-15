#include "reflex/ruby/event.h"


#include <rucy.h>
#include "rays/ruby/point.h"
#include "defs.h"


using namespace Rucy;

using Reflex::coord;


RUCY_DEFINE_VALUE_FROM_TO(Reflex::ScrollEvent)

#define THIS  to<Reflex::ScrollEvent*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::ScrollEvent, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Reflex::ScrollEvent>(klass);
}
RUCY_END

static
RUCY_DEFN(initialize)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "ScrollEvent#initialize", argc, 0, 1, 2, 3, 4, 5, 6);

	THIS->x       = (argc >= 1) ? to<coord>(argv[0]) : 0;
	THIS->y       = (argc >= 2) ? to<coord>(argv[1]) : 0;
	THIS->z       = (argc >= 3) ? to<coord>(argv[2]) : 0;
	THIS->dx      = (argc >= 4) ? to<coord>(argv[3]) : 0;
	THIS->dy      = (argc >= 5) ? to<coord>(argv[4]) : 0;
	THIS->dz      = (argc >= 6) ? to<coord>(argv[5]) : 0;

	return rb_call_super(0, NULL);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::ScrollEvent&>(obj);
	return self;
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
RUCY_DEF0(scroll)
{
	CHECK;
	return value(Rays::Point(THIS->x, THIS->y, THIS->z));
}
RUCY_END

static
RUCY_DEF0(delta)
{
	CHECK;
	return value(Rays::Point(THIS->dx, THIS->dy, THIS->dz));
}
RUCY_END


static Class cScrollEvent;

void
Init_scroll_event ()
{
	Module mReflex = define_module("Reflex");

	cScrollEvent = mReflex.define_class("ScrollEvent", Reflex::event_class());
	cScrollEvent.define_alloc_func(alloc);
	cScrollEvent.define_private_method("initialize", initialize);
	cScrollEvent.define_private_method("initialize_copy", initialize_copy);
	cScrollEvent.define_method("x", x);
	cScrollEvent.define_method("y", y);
	cScrollEvent.define_method("z", z);
	cScrollEvent.define_method("dx", dx);
	cScrollEvent.define_method("dy", dy);
	cScrollEvent.define_method("dz", dz);
	cScrollEvent.define_method("scroll", scroll);
	cScrollEvent.define_method("delta", delta);
}


namespace Reflex
{


	Class
	scroll_event_class ()
	{
		return cScrollEvent;
	}


}// Reflex
