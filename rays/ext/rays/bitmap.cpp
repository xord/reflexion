#include "rays/ruby/bitmap.h"


#include <rucy.h>
#include "rays/exception.h"
#include "rays/ruby/color_space.h"
#include "rays/ruby/color.h"
#include "rays/ruby/font.h"
#include "defs.h"


using namespace Rucy;

using Rays::uchar;
using Rays::ushort;
using Rays::uint;
using Rays::coord;


RUCY_DEFINE_VALUE_FROM_TO(Rays::Bitmap)

#define THIS  to<Rays::Bitmap*>(self)

#define CHECK RUCY_CHECK_OBJECT(Rays::Bitmap, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::Bitmap>(klass);
}
RUCY_END

static
RUCY_DEFN(initialize)
{
	RUCY_CHECK_OBJ(Rays::Bitmap, self);
	check_arg_count(__FILE__, __LINE__, "Bitmap#initialize", argc, 2, 3);

	*THIS = Rays::Bitmap(
		to<int>(argv[0]), to<int>(argv[1]),
		argc >= 3 ? to<Rays::ColorSpace>(argv[2]) : Rays::RGBA);

	return self;
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	RUCY_CHECK_OBJ(Rays::Bitmap, self);

	*THIS = to<Rays::Bitmap&>(obj).copy();
	return self;
}
RUCY_END

static
RUCY_DEF0(width)
{
	CHECK;

	return value(THIS->width());
}
RUCY_END

static
RUCY_DEF0(height)
{
	CHECK;

	return value(THIS->height());
}
RUCY_END

static
RUCY_DEF0(color_space)
{
	CHECK;

	return value(THIS->color_space());
}
RUCY_END

static
RUCY_DEFN(set_at)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Bitmap#set_at", argc, 3, 4, 5, 6);

	int x = argv[0].to_i();
	int y = argv[1].to_i();
	Rays::Color color = to<Rays::Color>(argc - 2, argv + 2);

	color.get(THIS->at<void>(x, y), THIS->color_space());
	return color;
}
RUCY_END

static
RUCY_DEF2(get_at, x, y)
{
	CHECK;

	return value(Rays::Color(THIS->at<void>(x.as_i(), y.as_i()), THIS->color_space()));
}
RUCY_END


static Class cBitmap;

void
Init_bitmap ()
{
	Module mRays = define_module("Rays");

	cBitmap = mRays.define_class("Bitmap");
	cBitmap.define_alloc_func(alloc);
	cBitmap.define_private_method("initialize",      initialize);
	cBitmap.define_private_method("initialize_copy", initialize_copy);
	cBitmap.define_method("width", width);
	cBitmap.define_method("height", height);
	cBitmap.define_method("color_space", color_space);
	cBitmap.define_method("[]=", set_at);
	cBitmap.define_method("[]",  get_at);
}


namespace Rays
{


	Class
	bitmap_class ()
	{
		return cBitmap;
	}


}// Rays
