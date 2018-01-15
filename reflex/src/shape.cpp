#include "shape.h"


#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>
#include <Box2D/Collision/Shapes/b2ChainShape.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <xot/util.h>
#include "reflex/exception.h"
#include "reflex/debug.h"
#include "selector.h"
#include "view.h"
#include "world.h"
#include "body.h"
#include "fixture.h"


namespace Reflex
{


	static bool
	has_fill (Shape* shape)
	{
		assert(shape);

		View* owner = shape->owner();
		if (!owner) return false;

		const Style& style = View_get_style(owner);
		bool default_shape = shape == owner->shape(false);
		return default_shape ? style.background_fill() : style.foreground_fill();
	}


	struct Shape::Data
	{

		enum Flags
		{

			UPDATE_FIXTURES = Xot::bit(0),

			DEFAULT_FLAGS   = 0

		};// Flags

		View* owner;

		uint flags;

		SelectorPtr              pselector;

		std::unique_ptr<Bounds>  pframe;

		std::unique_ptr<Fixture> pfixtures;

		Data ()
		:	owner(NULL), flags(DEFAULT_FLAGS)
		{
		}

		Fixture* fixtures (Shape* shape)
		{
			assert(shape);

			if (!pfixtures)
			{
				Body* b    = View_get_body(owner);
				Fixture* f = (b && !Body_is_temporary(b))
					? shape->create_fixtures()
					: Fixture_create_temporary();
				assert(f);

				pfixtures.reset(f);
			}
			return pfixtures.get();
		}

		void update_fixtures (Shape* shape)
		{
			assert(shape);

			Body* body = View_get_body(owner, false);
			if (!pfixtures && !body)
				return;

			Fixture* fix = fixtures(shape);
			if (Fixture_is_temporary(fix) && Body_is_temporary(body))
				return;

			std::unique_ptr<Fixture> old = std::move(pfixtures);
			Fixture_copy_attributes(old.get(), fixtures(shape));
		}

	};// Shape::Data


	bool
	Shape_set_owner (Shape* shape, View* owner)
	{
		if (!shape)
			argument_error(__FILE__, __LINE__);

		Shape::Data* self = shape->self.get();

		if (owner == self->owner)
			return false;

		if (owner && self->owner)
			invalid_state_error(__FILE__, __LINE__);

		self->owner = owner;
		return true;
	}

	void
	Shape_update_fixtures (Shape* shape, bool force)
	{
		if (!shape)
			argument_error(__FILE__, __LINE__);

		Shape::Data* self = shape->self.get();

		bool update = Xot::check_and_remove_flag(&self->flags, Shape::Data::UPDATE_FIXTURES);
		if (update || force)
			self->update_fixtures(shape);
	}

	void
	Shape_update_fixtures_on_next_update (Shape* shape)
	{
		assert(shape);

		if (!shape->owner()) return;

		Xot::add_flag(&shape->self->flags, Shape::Data::UPDATE_FIXTURES);
		View_update_shapes(shape->owner());
	}

	void
	Shape_call_contact_event (Shape* shape, const ContactEvent& event)
	{
		if (!shape)
			argument_error(__FILE__, __LINE__);

		ContactEvent e = event;
		shape->on_contact(&e);

		switch (e.type)
		{
			case ContactEvent::BEGIN: shape->on_contact_begin(&e); break;
			case ContactEvent::END:   shape->on_contact_end(&e);   break;
			case ContactEvent::NONE: break;
		}

		if (e.is_blocked())
			return;

		View_call_contact_event(shape->owner(), e);
	}


	Shape::Shape (const char* name)
	{
		if (name) set_name(name);
	}

	Shape::~Shape ()
	{
	}

	View*
	Shape::owner ()
	{
		return self->owner;
	}

	const View*
	Shape::owner () const
	{
		return const_cast<Shape*>(this)->owner();
	}

	void
	Shape::set_frame (coord x, coord y, coord width, coord height)
	{
		set_frame(Bounds(x, y, width, height));
	}

	void
	Shape::set_frame (const Bounds& frame)
	{
		if (self->pframe)
			*self->pframe = frame;
		else
			self->pframe.reset(new Bounds(frame));

		Shape_update_fixtures_on_next_update(this);
	}

	static Bounds
	get_owner_frame (const Shape* shape)
	{
		assert(shape);

		if (!shape->owner())
			return 0;

		return shape->owner()->frame().dup().move_to(0, 0);
	}

	Bounds
	Shape::frame () const
	{
		return self->pframe ? *self->pframe : get_owner_frame(this);
	}

	void
	Shape::set_density (float density)
	{
		if (density == this->density()) return;

		self->fixtures(this)->set_density(density);
	}

	float
	Shape::density () const
	{
		const Fixture* f = self->pfixtures.get();
		return f ? f->density() : 0;
	}

	void
	Shape::set_friction (float friction)
	{
		if (friction == this->friction()) return;

		self->fixtures(this)->set_friction(friction);
	}

	float
	Shape::friction () const
	{
		const Fixture* f = self->pfixtures.get();
		return f ? f->friction() : 0;
	}

	void
	Shape::set_restitution (float restitution)
	{
		if (restitution == this->restitution()) return;

		self->fixtures(this)->set_restitution(restitution);
	}

	float
	Shape::restitution () const
	{
		const Fixture* f = self->pfixtures.get();
		return f ? f->restitution() : 0;
	}

	void
	Shape::set_sensor (bool state)
	{
		if (state == is_sensor()) return;

		self->fixtures(this)->set_sensor(state);
	}

	bool
	Shape::is_sensor () const
	{
		const Fixture* f = self->pfixtures.get();
		return f ? f->is_sensor() : false;
	}

	void
	Shape::set_category_bits (uint bits)
	{
		if (bits == category_bits()) return;

		self->fixtures(this)->set_category_bits(bits);
	}

	uint
	Shape::category_bits () const
	{
		const Fixture* f = self->pfixtures.get();
		return f ? f->category_bits() : 0x1;
	}

	void
	Shape::set_collision_mask (uint mask)
	{
		if (mask == collision_mask()) return;

		self->fixtures(this)->set_collision_mask(mask);
	}

	uint
	Shape::collision_mask () const
	{
		const Fixture* f = self->pfixtures.get();
		return f ? f->collision_mask() : 0xffff;
	}

	void
	Shape::on_attach (Event* e)
	{
		assert(owner());

		Shape_update_fixtures_on_next_update(this);
	}

	void
	Shape::on_detach (Event* e)
	{
		assert(!owner());

		self->update_fixtures(this);
	}

	void
	Shape::on_resize (FrameEvent* e)
	{
		if (!self->pframe)
			Shape_update_fixtures_on_next_update(this);
	}

	void
	Shape::on_contact (ContactEvent* e)
	{
	}

	void
	Shape::on_contact_begin (ContactEvent* e)
	{
	}

	void
	Shape::on_contact_end (ContactEvent* e)
	{
	}

	SelectorPtr*
	Shape::get_selector_ptr ()
	{
		return &self->pselector;
	}


	struct LineShape::Data
	{

		std::vector<Point> points;

		bool loop;

		Data ()
		:	loop(false)
		{
		}

	};// LineShape::Data


	LineShape::LineShape (const char* name)
	:	Super(name)
	{
	}

	LineShape::~LineShape ()
	{
	}

	void
	LineShape::add_point (coord x, coord y, coord z)
	{
		add_point(Point(x, y, z));
	}

	void
	LineShape::add_point (const Point& point)
	{
		self->points.emplace_back(point);

		Shape_update_fixtures_on_next_update(this);
	}

	void
	LineShape::add_points (const Point* points, size_t size)
	{
		self->points.insert(self->points.begin(), points, points + size);

		Shape_update_fixtures_on_next_update(this);
	}

	void
	LineShape::set_loop (bool loop)
	{
		self->loop = loop;
	}

	bool
	LineShape::loop () const
	{
		return self->loop;
	}

	void
	LineShape::on_draw (DrawEvent* e)
	{
		assert(e && e->painter);

		e->painter->line(&self->points[0], self->points.size(), self->loop);
	}

	static Fixture*
	create_line (LineShape* shape, float ppm)
	{
		assert(shape && shape->self->points.size() == 2);

		b2EdgeShape b2shape;
		b2shape.Set(
			to_b2vec2(shape->self->points[0], ppm),
			to_b2vec2(shape->self->points[1], ppm));

		return FixtureBuilder(shape, &b2shape).fixtures();
	}

	static Fixture*
	create_lines (LineShape* shape, float ppm)
	{
		assert(shape && shape->self->points.size() >= 3);
		LineShape::Data* self = shape->self.get();

		std::vector<b2Vec2> vecs;
		vecs.reserve(self->points.size());
		for (auto point : self->points)
			vecs.emplace_back(to_b2vec2(point, ppm));

		FixtureBuilder builder(shape);
		if (has_fill(shape))
		{
			b2PolygonShape b2shape;
			b2shape.Set(&vecs[0], (int32) vecs.size());
			builder.add(&b2shape);
		}
		else
		{
			b2ChainShape b2shape;
			if (self->loop)
				b2shape.CreateLoop(&vecs[0], (int32) vecs.size());
			else
				b2shape.CreateChain(&vecs[0], (int32) vecs.size());
			builder.add(&b2shape);
		}
		return builder.fixtures();
	}

	Fixture*
	LineShape::create_fixtures ()
	{
		const View* view = owner();
		if (!view)
			invalid_state_error(__FILE__, __LINE__);

		if (self->points.size() <= 1) return NULL;

		float ppm = view->meter2pixel();

		if (self->points.size() == 2)
			return create_line(this, ppm);
		else
			return create_lines(this, ppm);
	}


	struct RectShape::Data
	{

		coord
			round_left_top,    round_right_top,
			round_left_bottom, round_right_bottom;

		uint nsegment;

		Data ()
		:	round_left_top(0),    round_right_top(0),
			round_left_bottom(0), round_right_bottom(0),
			nsegment(0)
		{
		}

	};// RectShape::Data


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
		set_round(round, round, round, round);
	}

	void
	RectShape::set_round (
		coord left_top, coord right_top, coord left_bottom, coord right_bottom)
	{
		self->round_left_top     = left_top;
		self->round_right_top    = right_top;
		self->round_left_bottom  = left_bottom;
		self->round_right_bottom = right_bottom;

		Shape_update_fixtures_on_next_update(this);
	}

	void
	RectShape::set_round_left_top (coord round)
	{
		self->round_left_top = round;

		Shape_update_fixtures_on_next_update(this);
	}

	coord
	RectShape::round_left_top () const
	{
		return self->round_left_top;
	}

	void
	RectShape::set_round_right_top (coord round)
	{
		self->round_right_top = round;

		Shape_update_fixtures_on_next_update(this);
	}

	coord
	RectShape::round_right_top () const
	{
		return self->round_right_top;
	}

	void
	RectShape::set_round_left_bottom (coord round)
	{
		self->round_left_bottom = round;

		Shape_update_fixtures_on_next_update(this);
	}

	coord
	RectShape::round_left_bottom () const
	{
		return self->round_left_bottom;
	}

	void
	RectShape::set_round_right_bottom (coord round)
	{
		self->round_right_bottom = round;

		Shape_update_fixtures_on_next_update(this);
	}

	coord
	RectShape::round_right_bottom () const
	{
		return self->round_right_bottom;
	}

	void
	RectShape::set_nsegment (uint nsegment)
	{
		self->nsegment = nsegment;

		Shape_update_fixtures_on_next_update(this);
	}

	uint
	RectShape::nsegment () const
	{
		return self->nsegment;
	}

	void
	RectShape::on_draw (DrawEvent* e)
	{
		assert(e && e->painter);

		e->painter->rect(
			frame(),
			self->round_left_top,    self->round_right_top,
			self->round_left_bottom, self->round_right_bottom,
			self->nsegment);
	}

	static Fixture*
	create_solid_rect (RectShape* shape, const b2Vec2& pos, const b2Vec2& size)
	{
		float32 w = size.x / 2, h = size.y / 2;

		b2PolygonShape b2shape;
		b2shape.SetAsBox(w, h);
		for (int i = 0; i < 4; ++i)
		{
			b2shape.m_vertices[i].x += pos.x + w;
			b2shape.m_vertices[i].y += pos.y + h;
		}

		return FixtureBuilder(shape, &b2shape).fixtures();
	}

	static Fixture*
	create_line_rect (RectShape* shape, const b2Vec2& pos, const b2Vec2& size)
	{
		b2Vec2 points[] = {
			{pos.x,          pos.y},
			{pos.x + size.x, pos.y},
			{pos.x + size.x, pos.y + size.y},
			{pos.x,          pos.y + size.y}
		};

		b2ChainShape b2shape;
		b2shape.CreateLoop(points, 4);

		return FixtureBuilder(shape, &b2shape).fixtures();
	}

	Fixture*
	RectShape::create_fixtures ()
	{
		const View* view = owner();
		if (!view)
			invalid_state_error(__FILE__, __LINE__);

		Bounds f  = frame();
		float ppm = view->meter2pixel();
		b2Vec2 pos( to_b2coord(f.x, ppm), to_b2coord(f.y, ppm));
		b2Vec2 size(to_b2coord(f.w, ppm), to_b2coord(f.h, ppm));

		if (has_fill(this))
			return create_solid_rect(this, pos, size);
		else
			return create_line_rect(this, pos, size);
	}


	struct EllipseShape::Data
	{

		enum
		{
			DEFAULT_ANGLE_FROM = 0,
			DEFAULT_ANGLE_TO   = 360
		};

		float angle_from, angle_to;

		Point hole_size;

		uint nsegment;

		Data ()
		:	angle_from(DEFAULT_ANGLE_FROM), angle_to(DEFAULT_ANGLE_TO), nsegment(0)
		{
		}

		bool has_angle () const
		{
			return
				angle_from != DEFAULT_ANGLE_FROM ||
				angle_to   != DEFAULT_ANGLE_TO;
		}

		bool has_hole () const
		{
			return hole_size.x != 0 && hole_size.y != 0;
		}

	};// EllipseShape::Data


	EllipseShape::EllipseShape (const char* name)
	:	Super(name)
	{
	}

	EllipseShape::~EllipseShape ()
	{
	}

	void
	EllipseShape::set_angle_from (float degree)
	{
		self->angle_from = degree;

		Shape_update_fixtures_on_next_update(this);
	}

	float
	EllipseShape::angle_from () const
	{
		return self->angle_from;
	}

	void
	EllipseShape::set_angle_to (float degree)
	{
		self->angle_to = degree;

		Shape_update_fixtures_on_next_update(this);
	}

	float
	EllipseShape::angle_to () const
	{
		return self->angle_to;
	}

	void
	EllipseShape::set_hole_size (coord width, coord height)
	{
		self->hole_size.reset(width, height);

		Shape_update_fixtures_on_next_update(this);
	}

	void
	EllipseShape::set_hole_size (const Point& hole_size)
	{
		set_hole_size(hole_size.x, hole_size.y);
	}

	const Point&
	EllipseShape::hole_size () const
	{
		return self->hole_size;
	}

	void
	EllipseShape::set_nsegment (uint num_of_segments)
	{
		self->nsegment = num_of_segments;

		Shape_update_fixtures_on_next_update(this);
	}

	uint
	EllipseShape::nsegment () const
	{
		return self->nsegment;
	}

	void
	EllipseShape::on_draw (DrawEvent* e)
	{
		assert(e && e->painter);

		e->painter->ellipse(
			frame(), self->angle_from, self->angle_to,
			self->hole_size, self->nsegment);
	}

	static uint
	get_ellipse_nsegment (EllipseShape* shape)
	{
		assert(shape);
		EllipseShape::Data* self = shape->self.get();

		int nseg = self->nsegment;

		if (nseg <= 0)
		{
			nseg = Painter::NSEGMENT_ELLIPSE / 2;

			if (self->has_angle())
			{
				static const float ANGLE_MAX =
					EllipseShape::Data::DEFAULT_ANGLE_TO -
					EllipseShape::Data::DEFAULT_ANGLE_FROM;

				nseg *= (self->angle_to - self->angle_from) / ANGLE_MAX;
				if (nseg <  0) nseg = -nseg;
				if (nseg == 0) nseg = 1;
			}
		}

		return nseg;
	}

	static inline void
	make_ellipse_point (
		b2Vec2* result, const b2Vec2& center, const b2Vec2& radius,
		float angle_from, float angle_to, uint nsegment, uint segment_index)
	{
		assert(result);

		float t      = (float) segment_index / (float) nsegment;
		float radian = Xot::deg2rad(angle_from + (angle_to - angle_from) * t);
		result->Set(
			center.x + radius.x *  cos(radian),
			center.y + radius.y * -sin(radian));
	}

	static Fixture*
	create_solid_circle (EllipseShape* shape, const b2Vec2& center, coord radius)
	{
		assert(shape);

		b2CircleShape b2shape;
		b2shape.m_p      = center;
		b2shape.m_radius = radius;

		return FixtureBuilder(shape, &b2shape).fixtures();
	}

	static Fixture*
	create_solid_ellipse (
		EllipseShape* shape,
		const b2Vec2& center, const b2Vec2& radius,
		float angle_from, float angle_to, uint nseg)
	{
		assert(shape);

		FixtureBuilder builder(shape);

		b2Vec2 vecs[3];
		vecs[0] = center;
		for (uint i = 0; i < nseg; ++i)
		{
			make_ellipse_point(
				&vecs[1], center, radius, angle_from, angle_to, nseg, i + 0);
			make_ellipse_point(
				&vecs[2], center, radius, angle_from, angle_to, nseg, i + 1);

			b2PolygonShape b2shape;
			b2shape.Set(vecs, 3);

			builder.add(&b2shape);
		}

		return builder.fixtures();
	}

	static Fixture*
	create_line_ellipse (
		EllipseShape* shape,
		const b2Vec2& center, const b2Vec2& radius,
		uint nseg)
	{
		assert(shape);

		std::vector<b2Vec2> vecs(nseg);
		for (uint i = 0; i < nseg; ++i)
		{
			make_ellipse_point(
				&vecs[i], center, radius,
				EllipseShape::Data::DEFAULT_ANGLE_FROM,
				EllipseShape::Data::DEFAULT_ANGLE_TO,
				nseg, i);
		}

		b2ChainShape b2shape;
		b2shape.CreateLoop(&vecs[0], (int32) vecs.size());

		return FixtureBuilder(shape, &b2shape).fixtures();
	}

	static Fixture*
	create_line_ellipse (
		EllipseShape* shape,
		const b2Vec2& center, const b2Vec2& radius,
		float angle_from, float angle_to, uint nseg)
	{
		assert(shape);

		std::vector<b2Vec2> vecs(nseg + 2);
		for (uint i = 0; i <= nseg; ++i)
		{
			make_ellipse_point(
				&vecs[i], center, radius, angle_from, angle_to, nseg, i);
		}

		vecs.back() = center;

		b2ChainShape b2shape;
		b2shape.CreateLoop(&vecs[0], (int32) vecs.size());

		return FixtureBuilder(shape, &b2shape).fixtures();
	}

	static Fixture*
	create_solid_donut (
		EllipseShape* shape,
		const b2Vec2& center, const b2Vec2& radius_max, const b2Vec2& radius_min,
		float angle_from, float angle_to, uint nseg)
	{
		assert(shape);

		FixtureBuilder builder(shape);

		b2Vec2 vecs[4];
		for (uint i = 0; i < nseg; ++i)
		{
			make_ellipse_point(
				&vecs[0], center, radius_max, angle_from, angle_to, nseg, i + 0);
			make_ellipse_point(
				&vecs[1], center, radius_max, angle_from, angle_to, nseg, i + 1);
			make_ellipse_point(
				&vecs[2], center, radius_min, angle_from, angle_to, nseg, i + 1);
			make_ellipse_point(
				&vecs[3], center, radius_min, angle_from, angle_to, nseg, i + 0);

			b2PolygonShape b2shape;
			b2shape.Set(vecs, 4);

			builder.add(&b2shape);
		}

		return builder.fixtures();
	}

	static Fixture*
	create_line_donut (
		EllipseShape* shape,
		const b2Vec2& center, const b2Vec2& radius_max, const b2Vec2& radius_min,
		uint nseg)
	{
		assert(shape);

		FixtureBuilder builder(shape);

		std::vector<b2Vec2> vecs(nseg);
		for (const auto& radius : {radius_max, radius_min})
		{
			for (uint i = 0; i < nseg; ++i)
			{
				make_ellipse_point(
					&vecs[i], center, radius,
					EllipseShape::Data::DEFAULT_ANGLE_FROM,
					EllipseShape::Data::DEFAULT_ANGLE_TO,
					nseg, i);
			}

			b2ChainShape b2shape;
			b2shape.CreateLoop(&vecs[0], (int32) vecs.size());

			builder.add(&b2shape);
		}

		return builder.fixtures();
	}

	static Fixture*
	create_line_donut (
		EllipseShape* shape,
		const b2Vec2& center, const b2Vec2& radius_max, const b2Vec2& radius_min,
		float angle_from, float angle_to, uint nseg)
	{
		assert(shape);

		float from = angle_from, to = angle_to;

		uint npoint = nseg + 1;
		std::vector<b2Vec2> vecs(npoint * 2);
		for (uint i = 0; i <= nseg; ++i)
		{
			make_ellipse_point(
				&vecs[         i], center, radius_max, from, to, nseg,        i);
			make_ellipse_point(
				&vecs[npoint + i], center, radius_min, from, to, nseg, nseg - i);
		}

		b2ChainShape b2shape;
		b2shape.CreateLoop(&vecs[0], (int32) vecs.size());

		return FixtureBuilder(shape, &b2shape).fixtures();
	}

	Fixture*
	EllipseShape::create_fixtures ()
	{
		const View* view = owner();
		if (!view)
			invalid_state_error(__FILE__, __LINE__);

		Bounds f    = frame();
		float ppm   = view->meter2pixel();
		bool angled = self->has_angle();
		bool holed  = self->has_hole();
		bool solid  = has_fill(this);

		b2Vec2 center(to_b2coord(f.x, ppm),     to_b2coord(f.y, ppm));
		b2Vec2 radius(to_b2coord(f.w, ppm) / 2, to_b2coord(f.h, ppm) / 2);
		center += radius;

		if (radius.x == radius.y && !angled && !holed && solid)
			return create_solid_circle(this, center, radius.x);

		float from = self->angle_from;
		float to   = self->angle_to;
		uint nseg  = get_ellipse_nsegment(this);

		if (holed)
		{
			b2Vec2 min_(
				to_b2coord(self->hole_size.x, ppm) / 2,
				to_b2coord(self->hole_size.y, ppm) / 2);

			if (solid)
				return create_solid_donut(this, center, radius, min_, from, to, nseg);
			else if (angled)
				return create_line_donut(this, center, radius, min_, from, to, nseg);
			else
				return create_line_donut(this, center, radius, min_, nseg);
		}
		else if (solid)
			return create_solid_ellipse(this, center, radius, from, to, nseg);
		else if (angled)
			return create_line_ellipse(this, center, radius, from, to, nseg);
		else
			return create_line_ellipse(this, center, radius, nseg);
	}


	FixtureBuilder::FixtureBuilder (Shape* shape, const b2Shape* head)
	:	shape(shape), head(NULL), tail(NULL)
	{
		assert(shape);

		if (head) add(head);
	}

	void
	FixtureBuilder::add (const b2Shape* b2shape)
	{
		assert(b2shape);

		Body* body = View_get_body(shape->owner());
		if (!body)
			invalid_state_error(__FILE__, __LINE__);

		b2Body* b2body = Body_get_b2ptr(body);
		if (!b2body)
			invalid_state_error(__FILE__, __LINE__);

		b2Fixture* b2fix = b2body->CreateFixture(b2shape, 0);
		if (!b2fix)
			invalid_state_error(__FILE__, __LINE__);

		b2fix->SetUserData(shape);
		append(new Fixture(b2fix));
	}

	Fixture*
	FixtureBuilder::fixtures () const
	{
		return head;
	}

	void
	FixtureBuilder::append (Fixture* fixture)
	{
		if (!fixture) return;

		if (tail)
			tail->set_next(fixture);
		else
		{
			assert(!head && !tail);
			head = tail = fixture;
		}

		while (tail && tail->next())
			tail = tail->next();
	}


}// Reflex
