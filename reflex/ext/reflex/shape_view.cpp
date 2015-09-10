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
	return value(new Reflex::RubyView<Reflex::ShapeView>, klass);
}
RUCY_END


static Class cShapeView;

void
Init_shape_view ()
{
	Module mReflex = define_module("Reflex");

	cShapeView = mReflex.define_class("ShapeView", Reflex::view_class());
	cShapeView.define_alloc_func(alloc);
}


namespace Reflex
{


	Class
	shape_view_class ()
	{
		return cShapeView;
	}


}// Reflex
