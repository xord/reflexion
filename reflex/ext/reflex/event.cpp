#include "reflex/ruby/event.h"


#include "defs.h"


RUCY_DEFINE_VALUE_FROM_TO(Reflex::Event)

#define THIS  to<Reflex::Event*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::Event, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Reflex::Event>(klass);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::Event&>(obj);
	return self;
}
RUCY_END

static
RUCY_DEF0(block)
{
	CHECK;
	THIS->block();
}
RUCY_END

static
RUCY_DEF0(is_blocked)
{
	CHECK;
	return value(THIS->is_blocked());
}
RUCY_END


static Class cEvent;

void
Init_event ()
{
	Module mReflex = define_module("Reflex");

	cEvent = mReflex.define_class("Event");
	cEvent.define_alloc_func(alloc);
	cEvent.define_private_method("initialize_copy", initialize_copy);
	cEvent.define_method("block", block);
	cEvent.define_method("block?", is_blocked);
}


namespace Reflex
{


	Class
	event_class ()
	{
		return cEvent;
	}


}// Reflex
