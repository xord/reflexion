#include "reflex/ruby/shape_view.h"


#include <rucy.h>
#include <rays/ruby/point.h>


using namespace Rucy;

using Reflex::coord;


RUCY_DEFINE_WRAPPER_VALUE_FROM_TO(Reflex::RectShape)

#define THIS      to<Reflex::RectShape*>(self)

#define CHECK     RUCY_CHECK_OBJECT(Reflex::RectShape, self)

#define CALL(fun) RUCY_WRAPPER_CALL(Reflex::RectShape, THIS, fun)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return value(new Reflex::RubyShapeView<Reflex::RectShape>, klass);
}
RUCY_END

static
RUCY_DEFN(set_round)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "RectShape#set_fill", argc, 1, 2);

	if (argc == 1 && argv[0].is_kind_of(Rays::point_class()))
		THIS->set_round(to<Rays::Point&>(argv[0]));
	else
	{
		float w =               to<float>(argv[0]);
		float h = (argc >= 2) ? to<float>(argv[1]) : 1;
		THIS->set_round(w, h);
	}

	return self;
}
RUCY_END

static
RUCY_DEF0(get_round)
{
	CHECK;
	return value(THIS->round());
}
RUCY_END

static
RUCY_DEF_clear_override_flags(cof, Reflex::RectShape);


static Class cRectShape;

void
Init_rect_shape ()
{
	Module mReflex = define_module("Reflex");

	cRectShape = mReflex.define_class("RectShape", Reflex::shape_view_class());
	cRectShape.define_alloc_func(alloc);
	cRectShape.define_private_method("round=", set_round);
	cRectShape.define_method(        "round",  get_round);
	cRectShape.define_clear_override_flags(cof);
}


namespace Reflex
{


	Class
	rect_shape_class ()
	{
		return cRectShape;
	}


}// Reflex
