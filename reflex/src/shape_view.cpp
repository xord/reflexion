#include "reflex/shape_view.h"


#include <assert.h>
#include "reflex/exception.h"
#include "reflex/body.h"


namespace Reflex
{


	struct ShapeView::Data
	{

		Color fill, stroke;

		Data ()
		:	fill(1, 1), stroke(0, 0)
		{
		}

	};// ShapeView::Data


	ShapeView::ShapeView (const char* name)
	:	Super(name)
	{
	}

	ShapeView::~ShapeView ()
	{
	}

	void
	ShapeView::set_fill (float red, float green, float blue, float alpha)
	{
		self->fill.reset(red, green, blue, alpha);
	}

	void
	ShapeView::set_fill (const Color& color)
	{
		self->fill = color;
	}

	void
	ShapeView::no_fill ()
	{
		self->fill.alpha = 0;
	}

	const Color&
	ShapeView::fill () const
	{
		return self->fill;
	}

	void
	ShapeView::set_stroke (float red, float green, float blue, float alpha)
	{
		self->stroke.reset(red, green, blue, alpha);
	}

	void
	ShapeView::set_stroke (const Color& color)
	{
		self->stroke = color;
	}

	void
	ShapeView::no_stroke ()
	{
		self->stroke.alpha = 0;
	}

	const Color&
	ShapeView::stroke () const
	{
		return self->stroke;
	}

	Point
	ShapeView::content_size () const
	{
		return frame().size();
	}

	void
	ShapeView::on_draw (DrawEvent* e)
	{
		Color& fill   = self->fill;
		Color& stroke = self->stroke;
		if (fill.a <= 0 && stroke.a <= 0) return;

		assert(e && e->painter);
		Painter* p = e->painter;

		Color f = p->fill(), s = p->stroke();
		p->set_fill(fill);
		p->set_stroke(stroke);

		on_draw_shape(e);

		p->set_fill(f);
		p->set_stroke(s);
	}

	void
	ShapeView::on_draw_shape (DrawEvent* e)
	{
	}


	struct RectShape::Data
	{

		Point round;

	};// RectView::Data


	RectShape::RectShape (const char* name)
	:	Super(name)
	{
	}

	RectShape::~RectShape ()
	{
	}

	void
	RectShape::set_round (coord round)
	{
		set_round(round, round);
	}

	void
	RectShape::set_round (coord width, coord height)
	{
		self->round.reset(width, height);
	}

	void
	RectShape::set_round (const Point& round)
	{
		set_round(round.x, round.y);
	}

	const Point&
	RectShape::round () const
	{
		return self->round;
	}

	void
	RectShape::on_draw_shape (DrawEvent* e)
	{
		assert(e && e->painter);

		const Point& r = self->round;
		e->painter->rect(e->bounds, r.x, r.y);
	}


	struct EllipseShape::Data
	{

		coord radius_min;

		uint nsegment;

		Data ()
		:	radius_min(0), nsegment(0)
		{
		}

	};// EllipseView::Data


	EllipseShape::EllipseShape (const char* name)
	:	Super(name)
	{
	}

	EllipseShape::~EllipseShape ()
	{
	}

	void
	EllipseShape::set_radius_min (coord radius)
	{
		self->radius_min = radius;
	}

	coord
	EllipseShape::radius_min () const
	{
		return self->radius_min;
	}

	void
	EllipseShape::set_nsegment (uint num_of_segments)
	{
		self->nsegment = num_of_segments;
	}

	uint
	EllipseShape::nsegment () const
	{
		return self->nsegment;
	}

	void
	EllipseShape::make_body ()
	{
		Body* b = body();
		if (!b) return;

		b->clear_fixtures();

		const Point& size = frame().size();
		b->add_ellipse(size.x, size.y, self->radius_min, self->nsegment);
	}

	void
	EllipseShape::on_draw_shape (DrawEvent* e)
	{
		assert(e && e->painter);

		e->painter->ellipse(e->bounds, self->radius_min, self->nsegment);
	}


	struct ArcShape::Data
	{

		float angle_from, angle_to;

		Data ()
		:	angle_from(0), angle_to(360)
		{
		}

	};// ArcView::Data


	ArcShape::ArcShape (const char* name)
	:	Super(name)
	{
	}

	ArcShape::~ArcShape ()
	{
	}

	void
	ArcShape::set_angle_from (float degree)
	{
		self->angle_from = degree;
	}

	float
	ArcShape::angle_from () const
	{
		return self->angle_from;
	}

	void
	ArcShape::set_angle_to (float degree)
	{
		self->angle_to = degree;
	}

	float
	ArcShape::angle_to () const
	{
		return self->angle_to;
	}

	void
	ArcShape::make_body ()
	{
		Body* b = body();
		if (!b) return;

		b->clear_fixtures();

		const Point& size = frame().size();
		b->add_arc(
			size.x, size.y, self->angle_from, self->angle_to,
			radius_min(), nsegment());
	}

	void
	ArcShape::on_draw_shape (DrawEvent* e)
	{
		assert(e && e->painter);

		e->painter->arc(
			e->bounds, self->angle_from, self->angle_to,
			radius_min(), nsegment());
	}


}// Reflex
