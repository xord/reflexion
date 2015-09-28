#include "reflex/ruby/event.h"


#include <rucy.h>
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_VALUE_FROM_TO(Reflex::UpdateEvent)

#define THIS  to<Reflex::UpdateEvent*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::UpdateEvent, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Reflex::UpdateEvent>(klass);
}
RUCY_END

static
RUCY_DEFN(initialize)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "UpdateEvent#initialize", argc, 0, 1, 2);

	THIS->now = (argc >= 1) ? argv[0].as_f() : 0;
	THIS->dt  = (argc >= 2) ? argv[1].as_f() : 0;

	return rb_call_super(0, NULL);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::UpdateEvent&>(obj);
	return self;
}
RUCY_END

static
RUCY_DEF0(now)
{
	CHECK;
	return value(THIS->now);
}
RUCY_END

static
RUCY_DEF0(dt)
{
	CHECK;
	return value(THIS->dt);
}
RUCY_END


static Class cUpdateEvent;

void
Init_update_event ()
{
	Module mReflex = define_module("Reflex");

	cUpdateEvent = mReflex.define_class("UpdateEvent", Reflex::event_class());
	cUpdateEvent.define_alloc_func(alloc);
	cUpdateEvent.define_private_method("initialize",      initialize);
	cUpdateEvent.define_private_method("initialize_copy", initialize_copy);
	cUpdateEvent.define_method("now", now);
	cUpdateEvent.define_method("dt", dt);
}


namespace Reflex
{


	Class
	update_event_class ()
	{
		return cUpdateEvent;
	}


}// Reflex
