#include "reflex/ruby/event.h"


#include "reflex/ruby/view.h"
#include "defs.h"


RUCY_DEFINE_VALUE_FROM_TO(Reflex::FocusEvent)

#define THIS  to<Reflex::FocusEvent*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::FocusEvent, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Reflex::FocusEvent>(klass);
}
RUCY_END

static
RUCY_DEF3(initialize, type, current, last)
{
	CHECK;

	THIS->type    = (Reflex::FocusEvent::Type) to<uint>(type);
	THIS->current = to<Reflex::View*>(current);
	THIS->last    = to<Reflex::View*>(last);

	return rb_call_super(0, NULL);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::FocusEvent&>(obj);
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
RUCY_DEF0(get_current)
{
	CHECK;
	return THIS->current ? value(THIS->current) : nil();
}
RUCY_END

static
RUCY_DEF0(get_last)
{
	CHECK;
	return THIS->last ? value(THIS->last) : nil();
}
RUCY_END


static Class cFocusEvent;

void
Init_focus_event ()
{
	Module mReflex = define_module("Reflex");

	cFocusEvent = mReflex.define_class("FocusEvent", Reflex::event_class());
	cFocusEvent.define_alloc_func(alloc);
	cFocusEvent.define_private_method("initialize",      initialize);
	cFocusEvent.define_private_method("initialize_copy", initialize_copy);
	cFocusEvent.define_method("type",    get_type);
	cFocusEvent.define_method("current", get_current);
	cFocusEvent.define_method("last",    get_last);
	cFocusEvent.define_const("TYPE_NONE",  Reflex::FocusEvent::NONE);
	cFocusEvent.define_const("TYPE_FOCUS", Reflex::FocusEvent::FOCUS);
	cFocusEvent.define_const("TYPE_BLUR",  Reflex::FocusEvent::BLUR);
}


namespace Reflex
{


	Class
	focus_event_class ()
	{
		return cFocusEvent;
	}


}// Reflex
