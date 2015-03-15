#include "reflex/ruby/style_length.h"


#include <rucy.h>
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_VALUE_FROM_TO(Reflex::StyleLength)

#define THIS  to<Reflex::StyleLength*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::StyleLength, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Reflex::StyleLength>(klass);
}
RUCY_END

static
RUCY_DEF2(setup_num, value, unit)
{
	CHECK;

	const char* str = unit.as_s(true);
	Reflex::StyleLength::Unit u = Reflex::StyleLength::NONE;
	     if (strcasecmp(str, "px") == 0) u = Reflex::StyleLength::PIXEL;
	else if (strcasecmp(str, "%")  == 0) u = Reflex::StyleLength::PERCENT;
	else argument_error(__FILE__, __LINE__);
	THIS->reset(value.as_f(true), u);
}
RUCY_END

static
RUCY_DEF1(setup_str, str)
{
	CHECK;
	THIS->reset(str.c_str());
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::StyleLength&>(obj).copy();
	return self;
}
RUCY_END

static
RUCY_DEF0(get_value)
{
	CHECK;
	return value(THIS->value());
}
RUCY_END

static
RUCY_DEF0(get_unit)
{
	CHECK;

	RUCY_SYMBOL(pixel,   "px");
	RUCY_SYMBOL(percent, "%");
	switch (THIS->unit())
	{
		case Reflex::StyleLength::PIXEL:   return pixel.value();
		case Reflex::StyleLength::PERCENT: return percent.value();
		default:                           return nil();
	}
}
RUCY_END

static
RUCY_DEF0(to_s)
{
	CHECK;
	return value(THIS->to_s());
}
RUCY_END


static Class cStyleLength;

void
Init_style_length ()
{
	Module mReflex = define_module("Reflex");

	cStyleLength = mReflex.define_class("StyleLength");
	cStyleLength.define_alloc_func(alloc);
	cStyleLength.define_private_method("setup_num", setup_num);
	cStyleLength.define_private_method("setup_str", setup_str);
	cStyleLength.define_private_method("initialize_copy", initialize_copy);
	cStyleLength.define_method("value",  get_value);
	cStyleLength.define_method("unit",   get_unit);
	cStyleLength.define_method("to_s", to_s);
}


namespace Rucy
{


	template <> Reflex::StyleLength
	value_to<Reflex::StyleLength> (Value value, bool convert)
	{
		if (convert)
		{
			     if (value.is_nil()) return Reflex::StyleLength();
			else if (value.is_i())   return Reflex::StyleLength(value.as_i());
			else if (value.is_f())   return Reflex::StyleLength(value.as_f());
			else if (value.is_s())   return Reflex::StyleLength(value.c_str());
		}

		return value_to<Reflex::StyleLength&>(value, convert);
	}


}// Rucy


namespace Reflex
{


	Class
	style_length_class ()
	{
		return cStyleLength;
	}


}// Reflex
