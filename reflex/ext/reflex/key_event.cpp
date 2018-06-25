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
RUCY_DEFN(initialize)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "KeyEvent#initialize", argc, 0, 1, 2, 3, 4, 5);

	THIS->type      = (argc >= 1) ? (Reflex::KeyEvent::Type) to<int>(argv[0]) : Reflex::KeyEvent::NONE;
	THIS->chars     = (argc >= 2) ? argv[1].c_str()   : NULL;
	THIS->code      = (argc >= 3) ? to<int>(argv[2])  : Reflex::KEY_NONE;
	THIS->repeat    = (argc >= 4) ? to<int>(argv[3])  : 1;
	THIS->modifiers = (argc >= 5) ? to<uint>(argv[4]) : (uint) Reflex::MOD_NONE;

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
