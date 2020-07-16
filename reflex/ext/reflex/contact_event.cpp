#include "reflex/ruby/event.h"


#include "reflex/ruby/shape.h"
#include "reflex/ruby/view.h"
#include "defs.h"


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
RUCY_DEF2(initialize, type, shape)
{
	CHECK;

	THIS->type  = (Reflex::ContactEvent::Type) to<int>(type);
	THIS->shape = to<Reflex::Shape*>(shape);

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
RUCY_DEF0(type)
{
	CHECK;
	return value(THIS->type);
}
RUCY_END

static
RUCY_DEF0(shape)
{
	CHECK;
	return value(THIS->shape);
}
RUCY_END

static
RUCY_DEF0(view)
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
	cContactEvent.define_private_method("initialize",      initialize);
	cContactEvent.define_private_method("initialize_copy", initialize_copy);
	cContactEvent.define_method("type",  type);
	cContactEvent.define_method("shape", shape);
	cContactEvent.define_method("view",  view);
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
