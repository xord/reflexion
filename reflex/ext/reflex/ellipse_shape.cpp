#include "reflex/ruby/shape.h"


#include <rucy.h>
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_WRAPPER_VALUE_FROM_TO(Reflex::EllipseShape)

#define THIS      to<Reflex::EllipseShape*>(self)

#define CHECK     RUCY_CHECK_OBJ(Reflex::EllipseShape, self)

#define CALL(fun) RUCY_WRAPPER_CALL(Reflex::EllipseShape, THIS, fun)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return value(new Reflex::RubyShape<Reflex::EllipseShape>, klass);
}
RUCY_END

static
RUCY_DEF1(set_angle_from, degree)
{
	CHECK;
	THIS->set_angle_from(to<float>(degree));
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
	THIS->set_angle_to(to<float>(degree));
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
RUCY_DEF1(set_radius_min, radius)
{
	CHECK;
	THIS->set_radius_min(to<coord>(radius));
	return radius;
}
RUCY_END

static
RUCY_DEF0(get_radius_min)
{
	CHECK;
	return value(THIS->radius_min());
}
RUCY_END

static
RUCY_DEF1(set_nsegment, nsegment)
{
	CHECK;
	THIS->set_nsegment(to<uint>(nsegment));
	return nsegment;
}
RUCY_END

static
RUCY_DEF0(get_nsegment)
{
	CHECK;
	return value(THIS->nsegment());
}
RUCY_END

static
RUCY_DEF_clear_override_flags(cof, Reflex::EllipseShape);


static Class cEllipseShape;

void
Init_ellipse_shape ()
{
	Module mReflex = define_module("Reflex");

	cEllipseShape = mReflex.define_class("EllipseShape", Reflex::shape_class());
	cEllipseShape.define_alloc_func(alloc);
	cEllipseShape.define_method("angle_from=", set_angle_from);
	cEllipseShape.define_method("angle_from",  get_angle_from);
	cEllipseShape.define_method("angle_to=",   set_angle_to);
	cEllipseShape.define_method("angle_to=",   get_angle_to);
	cEllipseShape.define_method("radius_min=", set_radius_min);
	cEllipseShape.define_method("radius_min",  get_radius_min);
	cEllipseShape.define_method("nsegment=", set_nsegment);
	cEllipseShape.define_method("nsegment",  get_nsegment);
	cEllipseShape.define_clear_override_flags(cof);
}


namespace Reflex
{


	Class
	ellipse_shape_class ()
	{
		return cEllipseShape;
	}


}// Reflex
