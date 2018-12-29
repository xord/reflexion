#include "defs.h"


#include <assert.h>
#include "rays/ruby/bounds.h"
#include "rays/ruby/point.h"


void
get_line_args (std::vector<Rays::Point>* points, int argc, const Value* argv)
{
	assert(points && argv);

	points->clear();

	if (argc <= 0)
		return;

	if (argv[0].is_num())
	{
		if (argc % 2 != 0)
			argument_error(__FILE__, __LINE__);

		points->reserve(argc / 2);
		for (int i = 0; i < argc; i += 2)
		{
			coord x = to<coord>(argv[i + 0]);
			coord y = to<coord>(argv[i + 1]);
			points->emplace_back(Rays::Point(x, y));
		}
	}
	else
	{
		points->reserve(argc);
		for (int i = 0; i < argc; ++i)
			points->emplace_back(to<Rays::Point>(argv[i]));
	}
}

static uint
get_nsegment (Value nsegment)
{
	int value = nsegment ? to<int>(nsegment) : 0;
	if (value < 0) value = 0;
	return (uint) value;
}

void get_rect_args (
	coord* x, coord* y, coord* w, coord* h,
	coord* lt, coord* rt, coord* lb, coord* rb,
	uint* nseg,
	int argc, const Value* argv,
	Value round, Value lefttop, Value righttop, Value leftbottom, Value rightbottom,
	Value nsegment)
{
	assert(x && y && w && h && lt && rt && lb && rb && nseg && argv);

	if (argc <= 0)
		argument_error(__FILE__, __LINE__);

	if (argv[0].is_kind_of(Rays::bounds_class()))
	{
		Rays::Bounds& b = to<Rays::Bounds&>(argv[0]);
		*x  = b.x;
		*y  = b.y;
		*w  = b.w;
		*h  = b.h;
		*lt = argc >= 2 ? to<coord>(argv[1]) : 0;
		*rt = argc >= 3 ? to<coord>(argv[2]) : *lt;
		*lb = argc >= 4 ? to<coord>(argv[3]) : *lt;
		*rb = argc >= 5 ? to<coord>(argv[4]) : *lt;
	}
	else if (argv[0].is_kind_of(Rays::point_class()))
	{
		if (argc < 2)
			argument_error(__FILE__, __LINE__);

		Rays::Point& p = to<Rays::Point&>(argv[0]);
		*x  = p.x;
		*y  = p.y;
		*w  =             to<coord>(argv[1]);
		*h  = argc >= 3 ? to<coord>(argv[2]) : *w;
		*lt = argc >= 4 ? to<coord>(argv[3]) : 0;
		*rt = argc >= 5 ? to<coord>(argv[4]) : *lt;
		*lb = argc >= 6 ? to<coord>(argv[5]) : *lt;
		*rb = argc >= 7 ? to<coord>(argv[6]) : *lt;
	}
	else if (argc <= 2)
	{
		*x = *y = *lt = *rt = *lb = *rb = 0;
		*w =             to<coord>(argv[0]);
		*h = argc >= 2 ? to<coord>(argv[1]) : *w;
	}
	else
	{
		*x  =             to<coord>(argv[0]);
		*y  =             to<coord>(argv[1]);
		*w  =             to<coord>(argv[2]);
		*h  = argc >= 4 ? to<coord>(argv[3]) : *w;
		*lt = argc >= 5 ? to<coord>(argv[4]) : 0;
		*rt = argc >= 6 ? to<coord>(argv[5]) : *lt;
		*lb = argc >= 7 ? to<coord>(argv[6]) : *lt;
		*rb = argc >= 8 ? to<coord>(argv[7]) : *lt;
	}

	if (! lefttop)     lefttop    = round;
	if (!righttop)    righttop    = round;
	if (! leftbottom)  leftbottom = round;
	if (!rightbottom) rightbottom = round;

	if ( lefttop)    *lt = to<coord>( lefttop);
	if (righttop)    *rt = to<coord>(righttop);
	if ( leftbottom) *lb = to<coord>( leftbottom);
	if (rightbottom) *rb = to<coord>(rightbottom);

	*nseg = get_nsegment(nsegment);
}

void get_ellipse_args (
	coord* x, coord* y, coord* w, coord* h,
	Rays::Point* hole_size, float* from, float* to_,
	uint* nseg,
	int argc, const Value* argv,
	Value center, Value radius, Value hole, Value angle_from, Value angle_to,
	Value nsegment)
{
	assert(x && y && w && h && hole_size && from && to_ && nseg && argv);

	if (argc <= 0)
		argument_error(__FILE__, __LINE__);

	if (argv[0].is_kind_of(Rays::bounds_class()))
	{
		const Rays::Bounds& b = to<Rays::Bounds&>(argv[0]);
		*x = b.x;
		*y = b.y;
		*w = b.w;
		*h = b.h;
	}
	else if (argv[0].is_kind_of(Rays::point_class()))
	{
		if (argc < 2)
			argument_error(__FILE__, __LINE__);

		const Rays::Point& p = to<Rays::Point&>(argv[0]);
		*x = p.x;
		*y = p.y;
		*w =             to<coord>(argv[1]);
		*h = argc >= 3 ? to<coord>(argv[2]) : *w;
	}
	else if (argc <= 2)
	{
		*x = *y = 0;
		*w =             to<coord>(argv[0]);
		*h = argc >= 2 ? to<coord>(argv[1]) : *w;
	}
	else
	{
		*x =             to<coord>(argv[0]);
		*y =             to<coord>(argv[1]);
		*w =             to<coord>(argv[2]);
		*h = argc >= 4 ? to<coord>(argv[3]) : *w;
	}

	if (center)
	{
		Rays::Point p = to<Rays::Point>(center);
		*x = p.x;
		*y = p.y;
	}

	if (radius)
	{
		Rays::Point p = to<Rays::Point>(radius);
		*w = p.x * 2;
		*h = p.y * 2;
	}

	*hole_size = hole       ? to<Rays::Point>(hole) : 0;
	*from      = angle_from ? to<float>(angle_from) : 0;
	*to_       = angle_to   ? to<float>(angle_to)   : 360;
	*nseg      = get_nsegment(nsegment);
}
