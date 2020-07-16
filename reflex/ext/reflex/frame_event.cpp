#include "reflex/ruby/event.h"


#include "rays/ruby/point.h"
#include "rays/ruby/bounds.h"
#include "defs.h"


RUCY_DEFINE_VALUE_FROM_TO(Reflex::FrameEvent)

#define THIS  to<Reflex::FrameEvent*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::FrameEvent, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Reflex::FrameEvent>(klass);
}
RUCY_END

static
RUCY_DEF5(initialize, frame, dx, dy, dwidth, dheight)
{
	CHECK;

	THIS->frame   = to<Rays::Bounds>(frame);
	THIS->dx      = to<coord>(dx);
	THIS->dy      = to<coord>(dy);
	THIS->dwidth  = to<coord>(dwidth);
	THIS->dheight = to<coord>(dheight);

	return rb_call_super(0, NULL);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::FrameEvent&>(obj);
	return self;
}
RUCY_END

static
RUCY_DEF0(frame)
{
	CHECK;
	return value(THIS->frame);
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
RUCY_DEF0(dwidth)
{
	CHECK;
	return value(THIS->dwidth);
}
RUCY_END

static
RUCY_DEF0(dheight)
{
	CHECK;
	return value(THIS->dheight);
}
RUCY_END

static
RUCY_DEF0(dposition)
{
	CHECK;
	return value(Rays::Point(THIS->dx, THIS->dy));
}
RUCY_END

static
RUCY_DEF0(dsize)
{
	CHECK;
	return value(Rays::Point(THIS->dw, THIS->dh));
}
RUCY_END

static
RUCY_DEF0(angle)
{
	CHECK;
	return value(THIS->angle);
}
RUCY_END

static
RUCY_DEF0(dangle)
{
	CHECK;
	return value(THIS->dangle);
}
RUCY_END

static
RUCY_DEF0(is_move)
{
	CHECK;
	return value(THIS->is_move());
}
RUCY_END

static
RUCY_DEF0(is_resize)
{
	CHECK;
	return value(THIS->is_resize());
}
RUCY_END

static
RUCY_DEF0(is_rotate)
{
	CHECK;
	return value(THIS->is_rotate());
}
RUCY_END


static Class cFrameEvent;

void
Init_frame_event ()
{
	Module mReflex = define_module("Reflex");

	cFrameEvent = mReflex.define_class("FrameEvent", Reflex::event_class());
	cFrameEvent.define_alloc_func(alloc);
	cFrameEvent.define_private_method("initialize",      initialize);
	cFrameEvent.define_private_method("initialize_copy", initialize_copy);
	cFrameEvent.define_method("frame", frame);
	cFrameEvent.define_method("dx", dx);
	cFrameEvent.define_method("dy", dy);
	cFrameEvent.define_method("dwidth",  dwidth);
	cFrameEvent.define_method("dheight", dheight);
	cFrameEvent.define_method("dposition", dposition);
	cFrameEvent.define_method("dsize",     dsize);
	cFrameEvent.define_method( "angle",  angle);
	cFrameEvent.define_method("dangle", dangle);
	cFrameEvent.define_method("move?",   is_move);
	cFrameEvent.define_method("resize?", is_resize);
	cFrameEvent.define_method("rotate?", is_rotate);
}


namespace Reflex
{


	Class
	frame_event_class ()
	{
		return cFrameEvent;
	}


}// Reflex
