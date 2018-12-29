#include "rays/ruby/painter.h"


#include <vector>
#include "rays/ruby/point.h"
#include "rays/ruby/bounds.h"
#include "rays/ruby/color.h"
#include "rays/ruby/matrix.h"
#include "rays/ruby/image.h"
#include "rays/ruby/font.h"
#include "rays/ruby/shader.h"
#include "defs.h"


RUCY_DEFINE_VALUE_FROM_TO(Rays::Painter)

#define THIS  to<Rays::Painter*>(self)

#define CHECK RUCY_CHECK_OBJECT(Rays::Painter, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::Painter>(klass);
}
RUCY_END

static
RUCY_DEF4(canvas, x, y, width, height)
{
	CHECK;

	coord xx = to<coord>(x);
	coord yy = to<coord>(y);
	coord ww = to<coord>(width);
	coord hh = to<coord>(height);
	THIS->canvas(xx, yy, ww, hh);

	return self;
}
RUCY_END

static
RUCY_DEF0(bounds)
{
	CHECK;
	return value(THIS->bounds());
}
RUCY_END

static
RUCY_DEF0(pixel_density)
{
	CHECK;
	return value(THIS->pixel_density());
}
RUCY_END


static
RUCY_DEF0(begin_paint)
{
	CHECK;
	THIS->begin();
	return self;
}
RUCY_END

static
RUCY_DEF0(end_paint)
{
	CHECK;
	THIS->end();
	return self;
}
RUCY_END

static
RUCY_DEF0(clear)
{
	CHECK;
	THIS->clear();
}
RUCY_END

static
RUCY_DEF1(polygon, poly)
{
	CHECK;

	THIS->polygon(to<Rays::Polygon&>(poly));
	return self;
}
RUCY_END

static
RUCY_DEF2(line, args, loop)
{
	CHECK;

	std::vector<Rays::Point> points;
	get_line_args(&points, args.size(), args.as_array());

	THIS->line(&points[0], points.size(), loop);
	return self;
}
RUCY_END

static
RUCY_DEF1(polyline, poly)
{
	CHECK;

	THIS->line(to<Rays::Polyline&>(poly));
	return self;
}
RUCY_END

static
RUCY_DEF6(rect, args, round, lefttop, righttop, leftbottom, rightbottom)
{
	CHECK;

	coord x, y, w, h, lt, rt, lb, rb;
	uint _;
	get_rect_args(
		&x, &y, &w, &h, &lt, &rt, &lb, &rb, &_,
		args.size(), args.as_array(),
		round, lefttop, righttop, leftbottom, rightbottom, nil());

	THIS->rect(x, y, w, h, lt, rt, lb, rb);
	return self;
}
RUCY_END

static
RUCY_DEF6(ellipse, args, center, radius, hole, angle_from, angle_to)
{
	CHECK;

	coord x, y, w, h;
	Rays::Point hole_size;
	float from, to_;
	uint _;
	get_ellipse_args(
		&x, &y, &w, &h, &hole_size, &from, &to_, &_,
		args.size(), args.as_array(),
		center, radius, hole, angle_from, angle_to, nil());

	THIS->ellipse(x, y, w, h, hole_size, from, to_);
	return self;
}
RUCY_END

static
RUCY_DEFN(image)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Painter#image", argc, 1, 3, 5, 7, 9);

	const Rays::Image* image = to<Rays::Image*>(argv[0]);
	if (!image)
		argument_error(__FILE__, __LINE__, "%s is not an image.", argv[0].inspect().c_str());

	if (argc == 1)
		THIS->image(*image);
	else if (argc == 3)
	{
		coord x = to<coord>(argv[1]), y = to<coord>(argv[2]);
		THIS->image(*image, x, y);
	}
	else if (argc == 5)
	{
		coord x = to<coord>(argv[1]), w = to<coord>(argv[3]);
		coord y = to<coord>(argv[2]), h = to<coord>(argv[4]);
		THIS->image(*image, x, y, w, h);
	}
	else if (argc == 7)
	{
		coord sx = to<coord>(argv[1]), dx = to<coord>(argv[5]);
		coord sy = to<coord>(argv[2]), dy = to<coord>(argv[6]);
		coord sw = to<coord>(argv[3]);
		coord sh = to<coord>(argv[4]);
		THIS->image(*image, sx, sy, sw, sh, dx, dy);
	}
	else if (argc == 9)
	{
		coord sx = to<coord>(argv[1]), dx = to<coord>(argv[5]);
		coord sy = to<coord>(argv[2]), dy = to<coord>(argv[6]);
		coord sw = to<coord>(argv[3]), dw = to<coord>(argv[7]);
		coord sh = to<coord>(argv[4]), dh = to<coord>(argv[8]);
		THIS->image(*image, sx, sy, sw, sh, dx, dy, dw, dh);
	}

	return self;
}
RUCY_END

static
RUCY_DEFN(text)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Painter#text", argc, 1, 3, 5);

	if (argc == 1)
		THIS->text(argv[0].c_str());
	else if (argc == 3)
	{
		coord x = to<coord>(argv[1]), y = to<coord>(argv[2]);
		THIS->text(argv[0].c_str(), x, y);
	}
	else if (argc == 5)
	{
		coord x = to<coord>(argv[1]), w = to<coord>(argv[3]);
		coord y = to<coord>(argv[2]), h = to<coord>(argv[4]);
		THIS->text(argv[0].c_str(), x, y, w, h);
	}

	return self;
}
RUCY_END


static
RUCY_DEFN(set_background)
{
	CHECK;
	THIS->set_background(to<Rays::Color>(argc, argv));
	return self;
}
RUCY_END

static
RUCY_DEF0(get_background)
{
	CHECK;
	return value(THIS->background());
}
RUCY_END

static
RUCY_DEF0(no_background)
{
	CHECK;
	THIS->no_background();
	return self;
}
RUCY_END

static
RUCY_DEFN(set_fill)
{
	CHECK;
	THIS->set_fill(to<Rays::Color>(argc, argv));
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
RUCY_DEF0(no_fill)
{
	CHECK;
	THIS->no_fill();
	return self;
}
RUCY_END

static
RUCY_DEFN(set_stroke)
{
	CHECK;
	THIS->set_stroke(to<Rays::Color>(argc, argv));
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
RUCY_DEF0(no_stroke)
{
	CHECK;
	THIS->no_stroke();
	return self;
}
RUCY_END

static
RUCY_DEF1(set_stroke_width, width)
{
	CHECK;
	THIS->set_stroke_width(to<coord>(width));
	return self;
}
RUCY_END

static
RUCY_DEF0(get_stroke_width)
{
	CHECK;
	return value(THIS->stroke_width());
}
RUCY_END

static
RUCY_DEF1(set_nsegment, nsegment)
{
	CHECK;
	THIS->set_nsegment(nsegment ? to<int>(nsegment) : 0);
	return self;
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
RUCY_DEFN(set_clip)
{
	CHECK;
	THIS->set_clip(to<Rays::Bounds>(argc, argv));
	return self;
}
RUCY_END

static
RUCY_DEF0(get_clip)
{
	CHECK;
	return value(THIS->clip());
}
RUCY_END

static
RUCY_DEF0(no_clip)
{
	CHECK;
	THIS->no_clip();
	return self;
}
RUCY_END

static
RUCY_DEFN(set_font)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Painter#set_font", argc, 0, 1, 2);

	THIS->set_font(to<Rays::Font>(argc, argv));
	return self;
}
RUCY_END

static
RUCY_DEF0(get_font)
{
	CHECK;
	return value(THIS->font());
}
RUCY_END

static
RUCY_DEFN(set_shader)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Painter#set_shader", argc, 1);

	THIS->set_shader(to<Rays::Shader>(argc, argv));
	return self;
}
RUCY_END

static
RUCY_DEF0(get_shader)
{
	CHECK;
	return value(THIS->shader());
}
RUCY_END

static
RUCY_DEF0(no_shader)
{
	CHECK;
	THIS->no_shader();
	return self;
}
RUCY_END

static
RUCY_DEF0(push_state)
{
	CHECK;
	THIS->push_state();
	return self;
}
RUCY_END

static
RUCY_DEF0(pop_state)
{
	CHECK;
	THIS->pop_state();
	return self;
}
RUCY_END


static
RUCY_DEFN(translate)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Painter#translate", argc, 2, 3);

	coord xx = to<coord>(argv[0]);
	coord yy = to<coord>(argv[1]);
	coord zz = (argc >= 3) ? to<coord>(argv[2]) : 0;
	THIS->translate(xx, yy, zz);

	return self;
}
RUCY_END

static
RUCY_DEFN(scale)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Painter#scale", argc, 2, 3);

	coord xx = to<coord>(argv[0]);
	coord yy = to<coord>(argv[1]);
	coord zz = (argc >= 3) ? to<coord>(argv[2]) : 1;
	THIS->scale(xx, yy, zz);

	return self;
}
RUCY_END

static
RUCY_DEFN(rotate)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Painter#rotate", argc, 1, 3, 4);

	coord aa = to<coord>(argv[0]), xx = 0, yy = 0, zz = 1;
	if (argc >= 3)
	{
		xx = to<coord>(argv[1]);
		yy = to<coord>(argv[2]);
		zz = argc >= 4 ? to<coord>(argv[3]) : 0;
	}

	THIS->rotate(aa, xx, yy, zz);

	return self;
}
RUCY_END

static
RUCY_DEFN(set_matrix)
{
	CHECK;
	THIS->set_matrix(to<Rays::Matrix>(argc, argv));
	return value(THIS->matrix());
}
RUCY_END

static
RUCY_DEF0(get_matrix)
{
	CHECK;
	return value(THIS->matrix());
}
RUCY_END

static
RUCY_DEF0(push_matrix)
{
	CHECK;
	THIS->push_matrix();
	return self;
}
RUCY_END

static
RUCY_DEF0(pop_matrix)
{
	CHECK;
	THIS->pop_matrix();
	return self;
}
RUCY_END


static Class cPainter;

void
Init_painter ()
{
	Module mRays = define_module("Rays");

	cPainter = mRays.define_class("Painter");
	cPainter.define_alloc_func(alloc);

	cPainter.define_method("canvas", canvas);
	cPainter.define_method("bounds", bounds);
	cPainter.define_method("pixel_density", pixel_density);

	cPainter.define_private_method("begin_paint", begin_paint);
	cPainter.define_private_method(  "end_paint",   end_paint);
	cPainter.define_method("clear",   clear);
	cPainter.define_method("polygon", polygon);
	cPainter.define_private_method("draw_line",     line);
	cPainter.define_private_method("draw_polyline", polyline);
	cPainter.define_private_method("draw_rect",     rect);
	cPainter.define_private_method("draw_ellipse",  ellipse);
	cPainter.define_method("image", image);
	cPainter.define_method("text",  text);

	cPainter.define_method(   "background=", set_background);
	cPainter.define_method(   "background",  get_background);
	cPainter.define_method("no_background",   no_background);
	cPainter.define_method(   "fill=", set_fill);
	cPainter.define_method(   "fill",  get_fill);
	cPainter.define_method("no_fill",   no_fill);
	cPainter.define_method(   "stroke=", set_stroke);
	cPainter.define_method(   "stroke",  get_stroke);
	cPainter.define_method("no_stroke",   no_stroke);
	cPainter.define_method(   "stroke_width=", set_stroke_width);
	cPainter.define_method(   "stroke_width",  get_stroke_width);
	cPainter.define_method("nsegment=", set_nsegment);
	cPainter.define_method("nsegment",  get_nsegment);
	cPainter.define_method(   "clip=", set_clip);
	cPainter.define_method(   "clip",  get_clip);
	cPainter.define_method("no_clip",   no_clip);
	cPainter.define_method("font=", set_font);
	cPainter.define_method("font",  get_font);
	cPainter.define_private_method("set_shader", set_shader);
	cPainter.define_method(            "shader", get_shader);
	cPainter.define_method(         "no_shader",  no_shader);
	cPainter.define_method("push_state", push_state);
	cPainter.define_method( "pop_state",  pop_state);

	cPainter.define_method("translate", translate);
	cPainter.define_method("scale",     scale);
	cPainter.define_method("rotate",    rotate);
	cPainter.define_method("matrix=", set_matrix);
	cPainter.define_method("matrix",  get_matrix);
	cPainter.define_method("push_matrix", push_matrix);
	cPainter.define_method( "pop_matrix",  pop_matrix);
}


namespace Rays
{


	Class
	painter_class ()
	{
		return cPainter;
	}


}// Rays
