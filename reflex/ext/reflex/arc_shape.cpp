#include "reflex/ruby/shape_view.h"


#include <rucy.h>


using namespace Rucy;

using Reflex::coord;


RUCY_DEFINE_WRAPPER_VALUE_FROM_TO(Reflex::ArcShape)

#define THIS      to<Reflex::ArcShape*>(self)

#define CHECK     RUCY_CHECK_OBJECT(Reflex::ArcShape, self)

#define CALL(fun) RUCY_WRAPPER_CALL(Reflex::ArcShape, THIS, fun)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return value(new Reflex::RubyView<Reflex::ArcShape>, klass);
}
RUCY_END

static
RUCY_DEF1(set_angle_from, degree)
{
	CHECK;
	THIS->set_angle_from(degree.as_f(true));
	return self;
}
RUCY_END

static
RUCY_DEF0(get_angle_from)
{
	CHECK;
	return value(THIS->angle_from());
}
RUCY_END

static
RUCY_DEF1(set_angle_to, degree)
{
	CHECK;
	THIS->set_angle_to(degree.as_f(true));
	return self;
}
RUCY_END

static
RUCY_DEF0(get_angle_to)
{
	CHECK;
	return value(THIS->angle_to());
}
RUCY_END

static
RUCY_DEF_clear_override_flags(cof, Reflex::ArcShape);


static Class cArcShape;

void
Init_arc_shape ()
{
	Module mReflex = define_module("Reflex");

	cArcShape = mReflex.define_class("ArcShape", Reflex::ellipse_shape_class());
	cArcShape.define_alloc_func(alloc);
	cArcShape.define_method("angle_from=", set_angle_from);
	cArcShape.define_method("angle_from",  get_angle_from);
	cArcShape.define_method("angle_to=",   set_angle_to);
	cArcShape.define_method("angle_to=",   get_angle_to);
	cArcShape.define_clear_override_flags(cof);
}


namespace Reflex
{


	Class
	arc_shape_class ()
	{
		return cArcShape;
	}


}// Reflex
