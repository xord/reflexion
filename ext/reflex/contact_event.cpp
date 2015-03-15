#include "reflex/ruby/event.h"


#include <rucy.h>
#include "reflex/ruby/view.h"
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_VALUE_FROM_TO(Reflex::ContactEvent)

#define THIS  to<Reflex::ContactEvent*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::ContactEvent, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Reflex::ContactEvent>(klass);
}
RUCY_END

static
RUCY_DEFN(initialize)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "ContactEvent#initialize", argc, 0, 2);

	THIS->type = (argc >= 1) ? (Reflex::ContactEvent::Type) to<int>(argv[0]) : Reflex::ContactEvent::NONE;
	THIS->view = (argc >= 2) ? to<Reflex::View*>(argv[1]) : NULL;

	return rb_call_super(0, NULL);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::ContactEvent&>(obj);
	return self;
}
RUCY_END

static
RUCY_DEF0(get_type)
{
	CHECK;
	return value(THIS->type);
}
RUCY_END

static
RUCY_DEF0(get_view)
{
	CHECK;
	return value(THIS->view);
}
RUCY_END


static Class cContactEvent;

void
Init_contact_event ()
{
	Module mReflex = define_module("Reflex");

	cContactEvent = mReflex.define_class("ContactEvent", Reflex::event_class());
	cContactEvent.define_alloc_func(alloc);
	cContactEvent.define_private_method("initialize", initialize);
	cContactEvent.define_private_method("initialize_copy", initialize_copy);
	cContactEvent.define_method("get_type", get_type);
	cContactEvent.define_method("view",     get_view);
	cContactEvent.define_const("TYPE_NONE",  Reflex::ContactEvent::NONE);
	cContactEvent.define_const("TYPE_BEGIN", Reflex::ContactEvent::BEGIN);
	cContactEvent.define_const("TYPE_END",   Reflex::ContactEvent::END);
}


namespace Reflex
{


	Class
	contact_event_class ()
	{
		return cContactEvent;
	}


}// Reflex
