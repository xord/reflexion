#include "reflex/shape_view.h"


#include <assert.h>
#include "reflex/exception.h"
#include "reflex/body.h"


namespace Reflex
{


	ShapeView::ShapeView (const char* name)
	:	Super(name)
	{
		style(true)->set_fill(1);
	}

	ShapeView::~ShapeView ()
	{
	}

	Point
	ShapeView::content_size () const
	{
		return frame().size();
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
	RectShape::on_draw (DrawEvent* e)
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
	EllipseShape::on_draw (DrawEvent* e)
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
	ArcShape::on_draw (DrawEvent* e)
	{
		assert(e && e->painter);

		e->painter->arc(
			e->bounds, self->angle_from, self->angle_to,
			radius_min(), nsegment());
	}


}// Reflex
