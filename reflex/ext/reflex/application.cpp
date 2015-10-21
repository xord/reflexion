#include "reflex/ruby/application.h"


#include <rucy.h>
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_WRAPPER_VALUE_FROM_TO(Reflex::Application)

#define THIS      to<Reflex::Application*>(self)

#define CHECK_OBJ RUCY_CHECK_OBJ(   Reflex::Application, self)
#define CHECK     RUCY_CHECK_OBJECT(Reflex::Application, self)

#define CALL(fun) RUCY_WRAPPER_CALL(Reflex::Application, THIS, fun)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return value(new Reflex::RubyApplication<Reflex::Application>, klass);
}
RUCY_END

static
RUCY_DEF0(start)
{
	CHECK_OBJ;
	CALL(start());
	return self;
}
RUCY_END

static
RUCY_DEF0(quit)
{
	CHECK;
	CALL(quit());
	return self;
}
RUCY_END

static
RUCY_DEF1(set_name, name)
{
	CHECK;
	THIS->set_name(name.c_str());
	return name;
}
RUCY_END

static
RUCY_DEF0(get_name)
{
	CHECK;
	return value(THIS->name());
}
RUCY_END

static
RUCY_DEF1(on_start, event)
{
	CHECK;

	RUCY_SYM(call_start_block);
	self.call(call_start_block);

	CALL(on_start(to<Reflex::Event*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_quit, event)
{
	CHECK;
	CALL(on_quit(to<Reflex::Event*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_motion, event)
{
	CHECK;
	CALL(on_motion(to<Reflex::MotionEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_preference, event)
{
	CHECK;
	CALL(on_preference(to<Reflex::Event*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_about, event)
{
	CHECK;
	CALL(on_about(to<Reflex::Event*>(event)));
}
RUCY_END

static
RUCY_DEF0(instance)
{
	return value(Reflex::app());
}
RUCY_END


static Class cApplication;

void
Init_application ()
{
	Module mReflex = define_module("Reflex");

	cApplication = mReflex.define_class("Application");
	cApplication.define_alloc_func(alloc);
	cApplication.define_method("start", start);
	cApplication.define_method("quit",  quit);
	cApplication.define_method("name=", set_name);
	cApplication.define_method("name",  get_name);
	cApplication.define_method("on_start", on_start);
	cApplication.define_method("on_quit",  on_quit);
	cApplication.define_method("on_motion", on_motion);
	cApplication.define_method("on_preference", on_preference);
	cApplication.define_method("on_about",      on_about);
	cApplication.define_singleton_method("instance", instance);
}


namespace Reflex
{


	Class
	application_class ()
	{
		return cApplication;
	}


}// Reflex
