#include "reflex/ruby/style_length.h"


#include <rucy.h>
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_VALUE_FROM_TO(Reflex::StyleLength4)

#define THIS  to<Reflex::StyleLength4*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::StyleLength4, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Reflex::StyleLength4>(klass);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::StyleLength4&>(obj).copy();
	return self;
}
RUCY_END

static
RUCY_DEF1(set_left, left)
{
	CHECK;
	THIS->set_left(to<Reflex::StyleLength>(left));
}
RUCY_END

static
RUCY_DEF0(get_left)
{
	CHECK;
	return value(THIS->left());
}
RUCY_END

static
RUCY_DEF1(set_top, top)
{
	CHECK;
	THIS->set_top(to<Reflex::StyleLength>(top));
}
RUCY_END

static
RUCY_DEF0(get_top)
{
	CHECK;
	return value(THIS->top());
}
RUCY_END

static
RUCY_DEF1(set_right, right)
{
	CHECK;
	THIS->set_right(to<Reflex::StyleLength>(right));
}
RUCY_END

static
RUCY_DEF0(get_right)
{
	CHECK;
	return value(THIS->right());
}
RUCY_END

static
RUCY_DEF1(set_bottom, bottom)
{
	CHECK;
	THIS->set_bottom(to<Reflex::StyleLength>(bottom));
}
RUCY_END

static
RUCY_DEF0(get_bottom)
{
	CHECK;
	return value(THIS->bottom());
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


static Class cStyleLength4;

void
Init_style_length4 ()
{
	Module mReflex = define_module("Reflex");

	cStyleLength4 = mReflex.define_class("StyleLength4");
	cStyleLength4.define_alloc_func(alloc);
	cStyleLength4.define_private_method("initialize_copy", initialize_copy);
	cStyleLength4.define_method("left=",   set_left);
	cStyleLength4.define_method("left",    get_left);
	cStyleLength4.define_method("top=",    set_top);
	cStyleLength4.define_method("top",     get_top);
	cStyleLength4.define_method("right=",  set_right);
	cStyleLength4.define_method("right",   get_right);
	cStyleLength4.define_method("bottom=", set_bottom);
	cStyleLength4.define_method("bottom",  get_bottom);
	cStyleLength4.define_method("[]=", set_at);
	cStyleLength4.define_method("[]",  get_at);
}


namespace Rucy
{


	template <> Reflex::StyleLength4
	value_to<Reflex::StyleLength4> (Value value, bool convert)
	{
		if (convert && value.is_array())
		{
			Value* a = value.as_array();
			switch (value.size())
			{
				case 1: return Reflex::StyleLength4(
					to<Reflex::StyleLength>(a[0]));

				case 2: return Reflex::StyleLength4(
					to<Reflex::StyleLength>(a[0]),
					to<Reflex::StyleLength>(a[1]));

				case 3: return Reflex::StyleLength4(
					to<Reflex::StyleLength>(a[0]),
					to<Reflex::StyleLength>(a[1]),
					to<Reflex::StyleLength>(a[2]));

				case 4: return Reflex::StyleLength4(
					to<Reflex::StyleLength>(a[0]),
					to<Reflex::StyleLength>(a[1]),
					to<Reflex::StyleLength>(a[2]),
					to<Reflex::StyleLength>(a[3]));

				default: argument_error(__FILE__, __LINE__);
			}
		}

		return value_to<Reflex::StyleLength4&>(value, convert);
	}


}// Rucy


namespace Reflex
{


	Class
	style_length4_class ()
	{
		return cStyleLength4;
	}


}// Reflex
