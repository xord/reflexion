#include "reflex/ruby/event.h"


#include <rucy.h>
#include "rays/ruby/point.h"
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_VALUE_FROM_TO(Reflex::MotionEvent)

#define THIS  to<Reflex::MotionEvent*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::MotionEvent, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Reflex::MotionEvent>(klass);
}
RUCY_END

static
RUCY_DEFN(initialize)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "MotionEvent#initialize", argc, 0, 1);

	if (argc >= 1)
		THIS->gravity = to<Rays::Point>(argv[0]);

	return rb_call_super(0, NULL);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::MotionEvent&>(obj);
	return self;
}
RUCY_END

static
RUCY_DEF0(gravity)
{
	CHECK;
	return value(THIS->gravity);
}
RUCY_END


static Class cMotionEvent;

void
Init_motion_event ()
{
	Module mReflex = define_module("Reflex");

	cMotionEvent = mReflex.define_class("MotionEvent", Reflex::event_class());
	cMotionEvent.define_alloc_func(alloc);
	cMotionEvent.define_private_method("initialize",      initialize);
	cMotionEvent.define_private_method("initialize_copy", initialize_copy);
	cMotionEvent.define_method("gravity", gravity);
}


namespace Reflex
{


	Class
	motion_event_class ()
	{
		return cMotionEvent;
	}


}// Reflex
