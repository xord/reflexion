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

	Fixture*
	RectShape::create_fixtures ()
	{
		const View* view = owner();
		if (!view)
			invalid_state_error(__FILE__, __LINE__);

		Bounds f  = frame();
		float ppm = view->meter2pixel();
		b2Vec2 pos( to_b2coord(f.x,     ppm),     to_b2coord(f.y,      ppm));
		b2Vec2 size(to_b2coord(f.width, ppm) / 2, to_b2coord(f.height, ppm) / 2);

		b2PolygonShape b2shape;
		b2shape.SetAsBox(size.x, size.y);
		for (int i = 0; i < 4; ++i)
		{
			b2shape.m_vertices[i].x += pos.x + size.x;
			b2shape.m_vertices[i].y += pos.y + size.y;
		}

		return FixtureBuilder(this, &b2shape).fixtures();
	}


	struct EllipseShape::Data
	{

		enum
		{
			DEFAULT_ANGLE_FROM = 0,
			DEFAULT_ANGLE_TO   = 360
		};

		float angle_from, angle_to;

		coord radius_min;

		uint nsegment;

		Data ()
		:	angle_from(DEFAULT_ANGLE_FROM), angle_to(DEFAULT_ANGLE_TO),
			radius_min(0), nsegment(0)
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
			return radius_min > 0;
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
	EllipseShape::set_radius_min (coord radius)
	{
		self->radius_min = radius;

		Shape_update_fixtures_on_next_update(this);
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
			self->radius_min, self->nsegment);
	}

	static Fixture*
	create_circle_fixture (Shape* shape, const b2Vec2& pos, coord size)
	{
		assert(shape);

		coord radius = size / 2;

		b2CircleShape b2shape;
		b2shape.m_radius = radius;
		b2shape.m_p.Set(pos.x + radius, pos.y + radius);

		return FixtureBuilder(shape, &b2shape).fixtures();
	}

	static inline void
	make_ellipse_point (
		b2Vec2* result, uint segment, uint nsegment,
		const b2Vec2& pos, coord width, coord height,
		float angle_from, float angle_to)
	{
		assert(result);

		float t      = (float) segment / (float) nsegment;
		float radian = Xot::deg2rad(angle_from + (angle_to - angle_from) * t);
		result->Set(
			pos.x + width  + width  *  cos(radian),
			pos.y + height + height * -sin(radian));
	}

	static Fixture*
	create_ellipse_fixtures (
		Shape* shape, const b2Vec2& pos, const b2Vec2& size,
		float angle_from, float angle_to, coord radius_min, uint nsegment)
	{
		assert(shape);

		coord w = size.x / 2, h = size.y / 2;

		if (nsegment == 0)
			nsegment = Painter::NSEGMENT_ELLIPSE / 2;

		FixtureBuilder builder(shape);

		b2Vec2 vecs[3];
		vecs[0].Set(pos.x + w, pos.y + h);// center
		for (uint seg = 0; seg < nsegment; ++seg)
		{
			make_ellipse_point(
				&vecs[1], seg + 0, nsegment, pos, w, h, angle_from, angle_to);
			make_ellipse_point(
				&vecs[2], seg + 1, nsegment, pos, w, h, angle_from, angle_to);

			b2PolygonShape b2shape;
			b2shape.Set(vecs, 3);

			builder.add(&b2shape);
		}

		return builder.fixtures();
	}

	Fixture*
	EllipseShape::create_fixtures ()
	{
		const View* view = owner();
		if (!view)
			invalid_state_error(__FILE__, __LINE__);

		Bounds f  = frame();
		float ppm = view->meter2pixel();
		b2Vec2 pos( to_b2coord(f.x,     ppm), to_b2coord(f.y,      ppm));
		b2Vec2 size(to_b2coord(f.width, ppm), to_b2coord(f.height, ppm));

		if (size.x == size.y && !self->has_angle() && !self->has_hole())
			return create_circle_fixture(this, pos, size.x);
		else
		{
			coord radius_min = to_b2coord(self->radius_min, ppm);
			return create_ellipse_fixtures(
				this, pos, size,
				self->angle_from, self->angle_to, radius_min, self->nsegment);
		}
	}


	struct Line
	{

		std::vector<Point> points;

		bool loop;

		Line (const Point& p1, const Point& p2)
		:	loop(false)
		{
			points.push_back(p1);
			points.push_back(p2);
		}

		Line (const Point* points, size_t size, bool loop = false)
		:	points(points, points + size), loop(loop)
		{
		}

	};// Line


	struct LineShape::Data
	{

		std::vector<std::unique_ptr<Line>> lines;

	};// LineShape::Data


	LineShape::LineShape (const char* name)
	:	Super(name)
	{
	}

	LineShape::~LineShape ()
	{
	}

	void
	LineShape::add (coord x1, coord y1, coord x2, coord y2)
	{
		add(Point(x1, y1), Point(x2, y2));
	}

	void
	LineShape::add (const Point& p1, const Point& p2)
	{
		self->lines.emplace_back(new Line(p1, p2));

		Shape_update_fixtures_on_next_update(this);
	}

	void
	LineShape::add (const Point* points, size_t size, bool loop)
	{
		self->lines.emplace_back(new Line(points, size, loop));

		Shape_update_fixtures_on_next_update(this);
	}

	void
	LineShape::on_draw (DrawEvent* e)
	{
		assert(e && e->painter);

		for (auto it = self->lines.begin(), end = self->lines.end(); it != end; ++it)
		{
			const Line& line = **it;
			e->painter->lines(&line.points[0], line.points.size());
		}
	}

	static void
	create_line_fixture (
		FixtureBuilder* builder, const Point& p1, const Point& p2, float ppm)
	{
		assert(builder);

		b2EdgeShape b2shape;
		b2shape.Set(to_b2vec2(p1, ppm), to_b2vec2(p2, ppm));

		builder->add(&b2shape);
	}

	static void
	create_line_fixtures (FixtureBuilder* builder, const Line& line, float ppm)
	{
		assert(builder);

		std::vector<b2Vec2> vecs;
		vecs.reserve(line.points.size());
		for (auto it = line.points.begin(), end = line.points.end(); it != end; ++it)
			vecs.push_back(to_b2vec2(*it, ppm));

		b2ChainShape b2shape;
		if (line.loop)
			b2shape.CreateLoop(&vecs[0], (int32) line.points.size());
		else
			b2shape.CreateChain(&vecs[0], (int32) line.points.size());

		builder->add(&b2shape);
	}

	Fixture*
	LineShape::create_fixtures ()
	{
		const View* view = owner();
		if (!view)
			invalid_state_error(__FILE__, __LINE__);

		float ppm = view->meter2pixel();

		FixtureBuilder builder(this);
		for (auto it = self->lines.begin(), end = self->lines.end(); it != end; ++it)
		{
			const Line& line = **it;
			if (line.points.size() == 2)
				create_line_fixture(&builder, line.points[0], line.points[1], ppm);
			else
				create_line_fixtures(&builder, line, ppm);
		}

		return builder.fixtures();
	}


	struct PolygonShape::Data
	{

		std::vector<std::unique_ptr<Line>> lines;

	};// PolygonShape::Data


	PolygonShape::PolygonShape (const char* name)
	:	Super(name)
	{
	}

	PolygonShape::~PolygonShape ()
	{
	}

	void
	PolygonShape::add (const Point* points, size_t size)
	{
		self->lines.emplace_back(new Line(points, size));

		Shape_update_fixtures_on_next_update(this);
	}

	void
	PolygonShape::on_draw (DrawEvent* e)
	{
		assert(e && e->painter);

		for (auto it = self->lines.begin(), end = self->lines.end(); it != end; ++it)
		{
			const Line& line = **it;
			e->painter->polygon(&line.points[0], line.points.size());
		}
	}

	static void
	create_polygon_fixtures (FixtureBuilder* builder, const Line& line, float ppm)
	{
		assert(builder);

		if (line.points.size() < 3)
			argument_error(__FILE__, __LINE__);

		std::vector<b2Vec2> vecs;
		vecs.reserve(line.points.size());
		for (auto it = line.points.begin(), end = line.points.end(); it != end; ++it)
			vecs.push_back(to_b2vec2(*it, ppm));

		b2PolygonShape b2shape;
		b2shape.Set(&vecs[0], (int32) line.points.size());

		builder->add(&b2shape);
	}

	Fixture*
	PolygonShape::create_fixtures ()
	{
		const View* view = owner();
		if (!view)
			invalid_state_error(__FILE__, __LINE__);

		float ppm = view->meter2pixel();

		FixtureBuilder builder(this);
		for (auto it = self->lines.begin(), end = self->lines.end(); it != end; ++it)
			create_polygon_fixtures(&builder, **it, ppm);

		return builder.fixtures();
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
