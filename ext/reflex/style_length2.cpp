#include "reflex/ruby/style_length.h"


#include <rucy.h>
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_VALUE_FROM_TO(Reflex::StyleLength2)

#define THIS  to<Reflex::StyleLength2*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::StyleLength2, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Reflex::StyleLength2>(klass);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::StyleLength2&>(obj).copy();
	return self;
}
RUCY_END

static
RUCY_DEF1(set_width, width)
{
	CHECK;
	THIS->set_width(to<Reflex::StyleLength>(width));
}
RUCY_END

static
RUCY_DEF0(get_width)
{
	CHECK;
	return value(THIS->width());
}
RUCY_END

static
RUCY_DEF1(set_height, height)
{
	CHECK;
	THIS->set_height(to<Reflex::StyleLength>(height));
}
RUCY_END

static
RUCY_DEF0(get_height)
{
	CHECK;
	return value(THIS->height());
}
RUCY_END

static
RUCY_DEF2(set_at, index, length)
{
	CHECK;

	int i = index.as_i(), size = (int) THIS->size();
	while (i < 0) i += size;

	if (i >= size)
		index_error(__FILE__, __LINE__);

	(*THIS)[i] = to<Reflex::StyleLength>(length);
}
RUCY_END

static
RUCY_DEF1(get_at, index)
{
	CHECK;

	int i = index.as_i(), size = (int) THIS->size();
	while (i < 0) i += size;

	if (i >= size)
		index_error(__FILE__, __LINE__);

	return value((*THIS)[i]);
}
RUCY_END


static Class cStyleLength2;

void
Init_style_length2 ()
{
	Module mReflex = define_module("Reflex");

	cStyleLength2 = mReflex.define_class("StyleLength2");
	cStyleLength2.define_alloc_func(alloc);
	cStyleLength2.define_private_method("initialize_copy", initialize_copy);
	cStyleLength2.define_method("width=",  set_width);
	cStyleLength2.define_method("width",   get_width);
	cStyleLength2.define_method("height=", set_height);
	cStyleLength2.define_method("height",  get_height);
	cStyleLength2.define_method("[]=", set_at);
	cStyleLength2.define_method("[]",  get_at);
}


namespace Rucy
{


	template <> Reflex::StyleLength2
	value_to<Reflex::StyleLength2> (Value value, bool convert)
	{
		if (convert && value.is_array())
		{
			Value* a = value.as_array();
			switch (value.size())
			{
				case 1: return Reflex::StyleLength2(
					to<Reflex::StyleLength>(a[0]));

				case 2: return Reflex::StyleLength2(
					to<Reflex::StyleLength>(a[0]),
					to<Reflex::StyleLength>(a[1]));

				default: argument_error(__FILE__, __LINE__);
			}
		}

		return value_to<Reflex::StyleLength2&>(value, convert);
	}


}// Rucy


namespace Reflex
{


	Class
	style_length2_class ()
	{
		return cStyleLength2;
	}


}// Reflex
