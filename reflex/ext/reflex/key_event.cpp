#include "reflex/ruby/event.h"


#include "defs.h"


RUCY_DEFINE_VALUE_FROM_TO(Reflex::KeyEvent)

#define THIS  to<Reflex::KeyEvent*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::KeyEvent, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Reflex::KeyEvent>(klass);
}
RUCY_END

static
RUCY_DEF5(initialize, type, chars, code, repeat, modifiers)
{
	CHECK;

	THIS->type      = (Reflex::KeyEvent::Type) to<int>(type);
	THIS->chars     = chars.c_str();
	THIS->code      = to<int>(code);
	THIS->repeat    = to<int>(repeat);
	THIS->modifiers = to<uint>(modifiers);

	return rb_call_super(0, NULL);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::KeyEvent&>(obj);
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
RUCY_DEF0(chars)
{
	CHECK;
	return value(THIS->chars.c_str());
}
RUCY_END

static
RUCY_DEF0(code)
{
	CHECK;
	return value(THIS->code);
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
RUCY_DEF0(repeat)
{
	CHECK;
	return value(THIS->repeat);
}
RUCY_END

static
RUCY_DEF0(capture)
{
	CHECK;
	return value(THIS->capture);
}
RUCY_END


static Class cKeyEvent;

void
Init_key_event ()
{
	Module mReflex = define_module("Reflex");

	cKeyEvent = mReflex.define_class("KeyEvent", Reflex::event_class());
	cKeyEvent.define_alloc_func(alloc);
	cKeyEvent.define_private_method("initialize",      initialize);
	cKeyEvent.define_private_method("initialize_copy", initialize_copy);
	cKeyEvent.define_method("type", type);
	cKeyEvent.define_method("chars", chars);
	cKeyEvent.define_method("code", code);
	cKeyEvent.define_method("modifiers", modifiers);
	cKeyEvent.define_method("repeat", repeat);
	cKeyEvent.define_method("capture?", capture);
	cKeyEvent.define_const("TYPE_NONE", Reflex::KeyEvent::NONE);
	cKeyEvent.define_const("TYPE_DOWN", Reflex::KeyEvent::DOWN);
	cKeyEvent.define_const("TYPE_UP",   Reflex::KeyEvent::UP);
}


namespace Reflex
{


	Class
	key_event_class ()
	{
		return cKeyEvent;
	}


}// Reflex
