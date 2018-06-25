#include "reflex/ruby/event.h"


#include "rays/ruby/point.h"
#include "defs.h"


RUCY_DEFINE_VALUE_FROM_TO(Reflex::CaptureEvent)

#define THIS  to<Reflex::CaptureEvent*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::CaptureEvent, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Reflex::CaptureEvent>(klass);
}
RUCY_END

static
RUCY_DEFN(initialize)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "CaptureEvent#initialize", argc, 0, 1, 2);

	THIS->begin = (argc >= 1) ? to<uint>(argv[0]) : 0;
	THIS->end   = (argc >= 2) ? to<uint>(argv[1]) : 0;

	return rb_call_super(0, NULL);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::CaptureEvent&>(obj);
	return self;
}
RUCY_END

static
RUCY_DEF0(begin)
{
	CHECK;
	return value(THIS->begin);
}
RUCY_END

static
RUCY_DEF0(end)
{
	CHECK;
	return value(THIS->end);
}
RUCY_END


static Class cCaptureEvent;

void
Init_capture_event ()
{
	Module mReflex = define_module("Reflex");

	cCaptureEvent = mReflex.define_class("CaptureEvent", Reflex::event_class());
	cCaptureEvent.define_alloc_func(alloc);
	cCaptureEvent.define_private_method("initialize",      initialize);
	cCaptureEvent.define_private_method("initialize_copy", initialize_copy);
	cCaptureEvent.define_private_method("get_begin", begin);
	cCaptureEvent.define_private_method("get_end",   end);
}


namespace Reflex
{


	Class
	capture_event_class ()
	{
		return cCaptureEvent;
	}


}// Reflex
