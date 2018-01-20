#include "reflex/ruby/filter.h"


#include <rucy.h>
#include <rays/ruby/painter.h>
#include <rays/ruby/image.h>
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_WRAPPER_VALUE_FROM_TO(Reflex::Filter)

#define THIS      to<Reflex::Filter*>(self)

#define CHECK     RUCY_CHECK_OBJ(Reflex::Filter, self)

#define CALL(fun) RUCY_WRAPPER_CALL(Reflex::Filter, THIS, fun)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return value(new Reflex::RubyFilter<Reflex::Filter>, klass);
}
RUCY_END

static
RUCY_DEF1(initialize, arg)
{
	CHECK;

	THIS->set_shader(to<Rays::Shader>(arg));
}
RUCY_END

static
RUCY_DEF2(apply, painter, image)
{
	CHECK;
	CALL(apply(to<Rays::Painter*>(painter), to<const Rays::Image&>(image)));
}
RUCY_END

static
RUCY_DEF1(set_shader, shader)
{
	CHECK;
	THIS->set_shader(to<Rays::Shader>(shader));
}
RUCY_END

static
RUCY_DEF0(get_shader)
{
	CHECK;
	return value(THIS->shader());
}
RUCY_END

static
RUCY_DEF_clear_override_flags(cof, Reflex::Filter);


static Class cFilter;

void
Init_filter ()
{
	Module mReflex = define_module("Reflex");

	cFilter = mReflex.define_class("Filter");
	cFilter.define_alloc_func(alloc);
	cFilter.define_private_method("initialize", initialize);
	cFilter.define_method("apply", apply);
	cFilter.define_method("shader=", set_shader);
	cFilter.define_method("shader",  get_shader);
	cFilter.define_clear_override_flags(cof);
}


namespace Reflex
{


	Class
	filter_class ()
	{
		return cFilter;
	}


}// Reflex
