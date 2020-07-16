#include "reflex/ruby/event.h"


#include <rays/ruby/point.h>
#include "defs.h"


RUCY_DEFINE_VALUE_FROM_TO(Reflex::PointerEvent)

#define THIS  to<Reflex::PointerEvent*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::PointerEvent, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Reflex::PointerEvent>(klass);
}
RUCY_END

static
RUCY_DEF6(initialize, type, pointer_type, modifiers, count, drag, positions)
{
	CHECK;

	int size = positions.size();
	if (size <= 0 || Reflex::PointerEvent::MAX < size)
		argument_error(__FILE__, __LINE__);

	THIS->type         = (Reflex::PointerEvent::Type) to<int>(type);
	THIS->pointer_type = to<uint>(pointer_type);
	THIS->modifiers    = to<uint>(modifiers);
	THIS->count        = to<uint>(count);
	THIS->drag         = to<bool>(drag);
	THIS->size         = (size_t) size;

	for (int i = 0; i < size; ++i)
		THIS->positions[i] = to<Rays::Point>(positions[i]);

	return rb_call_super(0, NULL);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::PointerEvent&>(obj);
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
RUCY_DEF0(pointer_type)
{
	CHECK;
	return value(THIS->pointer_type);
}
RUCY_END

static
RUCY_DEF0(size)
{
	CHECK;
	return value(THIS->size);
}
RUCY_END

static
RUCY_DEF0(modifiers)
{
	CHECK;
	return value(THIS->modifiers);
}
RUCY_END

static
RUCY_DEF0(count)
{
	CHECK;
	return value(THIS->count);
}
RUCY_END

static
RUCY_DEF0(drag)
{
	CHECK;
	return value(THIS->drag);
}
RUCY_END

static
RUCY_DEF0(capture)
{
	CHECK;
	return value(THIS->capture);
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
RUCY_DEFN(position)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "PointerEvent#position", argc, 0, 1);

	size_t index = argc >= 1 ? to<int>(argv[0]) : 0;
	return value(THIS->position(index));
}
RUCY_END

static
RUCY_DEF1(array_get, index)
{
	CHECK;
	return value((*THIS)[to<int>(index)]);
}
RUCY_END


static Class cPointerEvent;

void
Init_pointer_event ()
{
	Module mReflex = define_module("Reflex");

	cPointerEvent = mReflex.define_class("PointerEvent", Reflex::event_class());
	cPointerEvent.define_alloc_func(alloc);
	cPointerEvent.define_private_method("initialize",      initialize);
	cPointerEvent.define_private_method("initialize_copy", initialize_copy);
	cPointerEvent.define_method("type", type);
	cPointerEvent.define_method("pointer_type", pointer_type);
	cPointerEvent.define_method("size", size);
	cPointerEvent.define_method("modifiers", modifiers);
	cPointerEvent.define_method("count", count);
	cPointerEvent.define_method("drag?", drag);
	cPointerEvent.define_method("capture?", capture);
	cPointerEvent.define_method("x", x);
	cPointerEvent.define_method("y", y);
	cPointerEvent.define_method("position", position);
	cPointerEvent.define_method("[]", array_get);
	cPointerEvent.define_const("TYPE_NONE", Reflex::PointerEvent::NONE);
	cPointerEvent.define_const("TYPE_DOWN", Reflex::PointerEvent::DOWN);
	cPointerEvent.define_const("TYPE_UP",   Reflex::PointerEvent::UP);
	cPointerEvent.define_const("TYPE_MOVE", Reflex::PointerEvent::MOVE);
	cPointerEvent.define_const("POINTER_NONE",         Reflex::POINTER_NONE);
	cPointerEvent.define_const("POINTER_MOUSE_LEFT",   Reflex::POINTER_MOUSE_LEFT);
	cPointerEvent.define_const("POINTER_MOUSE_RIGHT",  Reflex::POINTER_MOUSE_RIGHT);
	cPointerEvent.define_const("POINTER_MOUSE_MIDDLE", Reflex::POINTER_MOUSE_MIDDLE);
	cPointerEvent.define_const("POINTER_TOUCH",        Reflex::POINTER_TOUCH);
	cPointerEvent.define_const("POINTER_PEN",          Reflex::POINTER_PEN);
}


namespace Reflex
{


	Class
	pointer_event_class ()
	{
		return cPointerEvent;
	}


}// Reflex
