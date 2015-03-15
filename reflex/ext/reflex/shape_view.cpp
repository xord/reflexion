#include "reflex/ruby/shape_view.h"


#include <rucy.h>
#include <rays/ruby/color.h>
#include "reflex/ruby/view.h"


using namespace Rucy;

using Reflex::coord;


RUCY_DEFINE_WRAPPER_VALUE_FROM_TO(Reflex::ShapeView)

#define THIS      to<Reflex::ShapeView*>(self)

#define CHECK     RUCY_CHECK_OBJECT(Reflex::ShapeView, self)

#define CALL(fun) RUCY_WRAPPER_CALL(Reflex::ShapeView, THIS, fun)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return value(new Reflex::RubyShapeView<Reflex::ShapeView>, klass);
}
RUCY_END

static
RUCY_DEFN(set_fill)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "ShapeView#set_fill", argc, 1, 2, 3, 4);

	if (argc == 1 && argv[0].is_kind_of(Rays::color_class()))
		THIS->set_fill(to<Rays::Color&>(argv[0]));
	else if (argc == 1 || argc == 2)
	{
		float v = to<float>(argv[0]);
		float a = (argc >= 2) ? to<float>(argv[1]) : 1;
		THIS->set_fill(v, v, v, a);
	}
	else
	{
		float r = to<float>(argv[0]);
		float g = to<float>(argv[1]);
		float b = to<float>(argv[2]);
		float a = (argc == 4) ? to<float>(argv[3]) : 1;
		THIS->set_fill(r, g, b, a);
	}

	return self;
}
RUCY_END

static
RUCY_DEF0(no_fill)
{
	CHECK;
	THIS->no_fill();
	return self;
}
RUCY_END

static
RUCY_DEF0(get_fill)
{
	CHECK;
	return value(THIS->fill());
}
RUCY_END

static
RUCY_DEFN(set_stroke)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "ShapeView#set_stroke", argc, 1, 2, 3, 4);

	if (argc == 1 && argv[0].is_kind_of(Rays::color_class()))
		THIS->set_stroke(to<Rays::Color&>(argv[0]));
	else if (argc == 1 || argc == 2)
	{
		float v = to<float>(argv[0]);
		float a = (argc >= 2) ? to<float>(argv[1]) : 1;
		THIS->set_stroke(v, v, v, a);
	}
	else
	{
		float r = to<float>(argv[0]);
		float g = to<float>(argv[1]);
		float b = to<float>(argv[2]);
		float a = (argc == 4) ? to<float>(argv[3]) : 1;
		THIS->set_stroke(r, g, b, a);
	}

	return self;
}
RUCY_END

static
RUCY_DEF0(no_stroke)
{
	CHECK;
	THIS->no_stroke();
	return self;
}
RUCY_END

static
RUCY_DEF0(get_stroke)
{
	CHECK;
	return value(THIS->stroke());
}
RUCY_END

static
RUCY_DEF1(on_draw_shape, event)
{
	CHECK;
	CALL(on_draw_shape(to<Reflex::DrawEvent*>(event)));
}
RUCY_END

static
RUCY_DEF_clear_override_flags(cof, Reflex::ShapeView);


static Class cShapeView;

void
Init_shape_view ()
{
	Module mReflex = define_module("Reflex");

	cShapeView = mReflex.define_class("ShapeView", Reflex::view_class());
	cShapeView.define_alloc_func(alloc);
	cShapeView.define_private_method("set_fill", set_fill);
	cShapeView.define_method(         "no_fill",  no_fill);
	cShapeView.define_method(            "fill", get_fill);
	cShapeView.define_private_method("set_stroke", set_stroke);
	cShapeView.define_method(         "no_stroke",  no_stroke);
	cShapeView.define_method(            "stroke", get_stroke);
	cShapeView.define_method("on_draw_shape", on_draw_shape);
	cShapeView.define_clear_override_flags(cof);
}


namespace Reflex
{


	Class
	shape_view_class ()
	{
		return cShapeView;
	}


}// Reflex
