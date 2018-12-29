#include "shape.h"


#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>
#include <Box2D/Collision/Shapes/b2ChainShape.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <rays/polygon.h>
#include "reflex/exception.h"
#include "reflex/debug.h"
#include "selector.h"
#include "view.h"
#include "world.h"
#include "body.h"
#include "fixture.h"


namespace Reflex
{


	class FixtureBuilder
	{

		public:

			FixtureBuilder (Shape* shape, const b2Shape* head = NULL)
			:	shape(shape), head(NULL), tail(NULL)
			{
				assert(shape);

				if (head) add(head);
			}

			void add (const b2Shape* b2shape)
			{
				assert(b2shape);

				Body* body = View_get_body(shape->owner());
				if (!body)
					invalid_state_error(__FILE__, __LINE__);

				append(new Fixture(body, b2shape, shape));
			}

			Fixture* fixtures () const
			{
				return head;
			}

		private:

			Shape* shape;

			Fixture *head, *tail;

			void append (Fixture* fixture)
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

	};// FixtureBuilder


	static bool
	triangulate (Polygon::TrianglePointList* triangles, const Polygon& polygon)
	{
		class TriangulatablePolygon : public Polygon
		{
			public:
				using Polygon::triangulate;
		};

		return ((const TriangulatablePolygon&) polygon).triangulate(triangles);
	}

	static Fixture*
	create_empty_fixture (Shape* shape)
	{
		assert(shape);

		b2CircleShape b2shape;
		return FixtureBuilder(shape, &b2shape).fixtures();
	}


	struct Shape::Data
	{

		enum Flags
		{

			UPDATE_POLYGON = Xot::bit(0),

			DEFAULT_FLAGS  = 0

		};// Flags

		View* owner = NULL;

		uint flags  = DEFAULT_FLAGS;

		Polygon polygon;

		SelectorPtr              pselector;

		std::unique_ptr<Bounds>  pframe;

		std::unique_ptr<Fixture> pfixtures;

		void update_polygon (Shape* shape)
		{
			assert(shape);

			Polygon poly = shape->update_polygon();
			if (poly.self == polygon.self)
				return;

			polygon = poly;

			//clip_polygon(shape);
			update_fixtures(shape);
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

		void clip_polygon (Shape* shape)
		{
			assert(shape);

			if (!polygon) return;

			Bounds f = shape->frame();
			polygon = polygon & Rays::create_rect(0, 0, f.width, f.height);
		}

		Fixture* fixtures (Shape* shape)
		{
			assert(shape);

			if (!pfixtures)
			{
				Body* body   = View_get_body(owner);
				Fixture* fix = (body && !Body_is_temporary(body))
					? create_shape_fixtures(shape)
					: Fixture_create_temporary();
				assert(fix);

				pfixtures.reset(fix);
			}
			return pfixtures.get();
		}

		Fixture* create_shape_fixtures (Shape* shape)
		{
			Fixture* f = shape->create_fixtures();
			return f ? f : create_empty_fixture(shape);
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
	Shape_update_polygon (Shape* shape, bool force)
	{
		if (!shape)
			argument_error(__FILE__, __LINE__);

		bool update = Xot::check_and_remove_flag(
			&shape->self->flags, Shape::Data::UPDATE_POLYGON);

		if (update || force)
			shape->self->update_polygon(shape);
	}

	static void
	update_polygon_on_next_update (Shape* shape)
	{
		assert(shape);

		if (!shape->owner()) return;

		Xot::add_flag(&shape->self->flags, Shape::Data::UPDATE_POLYGON);
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

		update_polygon_on_next_update(this);
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

		update_polygon_on_next_update(this);
	}

	static void
	make_fixtures_inactive (Shape* shape)
	{
		assert(shape);

		shape->self->update_fixtures(shape);
	}

	void
	Shape::on_detach (Event* e)
	{
		assert(!owner());

		make_fixtures_inactive(this);
	}

	void
	Shape::on_draw (DrawEvent* e)
	{
		assert(e && e->painter);

		e->painter->polygon(self->polygon);
	}

	void
	Shape::on_resize (FrameEvent* e)
	{
		if (!self->pframe)
			update_polygon_on_next_update(this);
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

	static const Style*
	get_style (const Shape* shape)
	{
		assert(shape);

		const View* owner = shape->owner();
		if (!owner) return NULL;

		return &View_get_style(owner);
	}

	static bool
	is_default_shape (const Shape* shape)
	{
		assert(shape);

		const View* owner = shape->owner();
		if (!owner) return false;

		return shape == owner->shape();
	}

	static const Color*
	get_fill (const Shape* shape)
	{
		assert(shape);

		const Style* style = get_style(shape);
		if (!style) return NULL;

		return is_default_shape(shape)
			? &style->background_fill()
			: &style->foreground_fill();
	}

	static coord
	get_stroke_width (const Shape* shape)
	{
		assert(shape);

		const Style* style = get_style(shape);
		if (!style) return 0;

		return is_default_shape(shape)
			? style->background_stroke_width()
			: style->foreground_stroke_width();
	}

	static bool
	has_fill (const Shape* shape)
	{
		assert(shape);

		const Color* fill = get_fill(shape);
		return fill && *fill;
	}

	static bool
	has_stroke_width (const Shape* shape)
	{
		assert(shape);

		return get_stroke_width(shape) > 0;
	}

	static Fixture*
	create_solid_fixtures (Shape* shape, const Polygon& polygon, float ppm)
	{
		assert(shape);

		Polygon::TrianglePointList triangles;
		if (!triangulate(&triangles, polygon))
			return NULL;

		assert((triangles.size() % 3) == 0);

		if (triangles.empty())
			return NULL;

		std::vector<b2Vec2> b2points;
		b2points.reserve(triangles.size());
		for (const auto& point : triangles)
			b2points.emplace_back(to_b2vec2(point, ppm));

		FixtureBuilder builder(shape);
		for (size_t i = 0; i < b2points.size(); i += 3)
		{
			b2PolygonShape b2shape;
			b2shape.Set(&b2points[i], 3);
			if (b2shape.m_count != 3)
				continue;// skip too small triangle

			builder.add(&b2shape);
		}

		return builder.fixtures();
	}

	static void
	add_polyline (
		FixtureBuilder* builder, const Polyline& polyline, float ppm,
		std::vector<b2Vec2>* buffer)
	{
		assert(builder && buffer);

		buffer->clear();
		buffer->reserve(polyline.size());
		for (const auto& point : polyline)
			buffer->emplace_back(to_b2vec2(point, ppm));

		b2ChainShape b2shape;
		if (polyline.loop())
			b2shape.CreateLoop( &(*buffer)[0], (int32) buffer->size());
		else
			b2shape.CreateChain(&(*buffer)[0], (int32) buffer->size());

		builder->add(&b2shape);
	}

	static Fixture*
	create_line_fixtures (Shape* shape, const Polygon& polygon, float ppm)
	{
		assert(shape);

		FixtureBuilder builder(shape);

		std::vector<b2Vec2> b2points;
		for (const auto& polyline : polygon)
			add_polyline(&builder, polyline, ppm, &b2points);

		return builder.fixtures();
	}

	Fixture*
	Shape::create_fixtures ()
	{
		View* view = owner();
		if (!view)
			invalid_state_error(__FILE__, __LINE__);

		Polygon polygon = get_polygon_for_fixtures();
		if (!polygon || polygon.empty())
			return NULL;

		float ppm = view->meter2pixel();

		if (has_fill(this))
			return create_solid_fixtures(this, polygon, ppm);
		else if (has_stroke_width(this))
		{
			Polygon hole;
			if (!polygon.expand(&hole, -get_stroke_width(this)))
				return NULL;

			return create_solid_fixtures(this, polygon - hole, ppm);
		}
		else
			return create_line_fixtures(this, polygon, ppm);
	}

	Polygon
	Shape::get_polygon_for_fixtures () const
	{
		return self->polygon;
	}


	struct PolygonShape::Data
	{

		Polygon polygon;

	};// PolygonShape


	PolygonShape::PolygonShape (const char* name)
	:	Super(name)
	{
	}

	PolygonShape::~PolygonShape ()
	{
	}

	void
	PolygonShape::set_polygon (const Polygon& polygon)
	{
		self->polygon = polygon;

		update_polygon_on_next_update(this);
	}

	const Polygon&
	PolygonShape::polygon () const
	{
		return self->polygon;
	}

	Polygon
	PolygonShape::update_polygon () const
	{
		return self->polygon;
	}


	struct LineShape::Data
	{

		std::vector<Point> points;

		bool loop = false;

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

		update_polygon_on_next_update(this);
	}

	void
	LineShape::add_points (const Point* points, size_t size)
	{
		self->points.insert(self->points.begin(), points, points + size);

		update_polygon_on_next_update(this);
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

	Polygon
	LineShape::update_polygon () const
	{
		if (self->points.empty())
			return Polygon();

		return Rays::create_line(&self->points[0], self->points.size(), self->loop);
	}


	struct RectShape::Data
	{

		coord round_left_top     = 0;
		coord round_right_top    = 0;
		coord round_left_bottom  = 0;
		coord round_right_bottom = 0;

		uint nsegment = 0;

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

		update_polygon_on_next_update(this);
	}

	void
	RectShape::set_round_left_top (coord round)
	{
		self->round_left_top = round;

		update_polygon_on_next_update(this);
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

		update_polygon_on_next_update(this);
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

		update_polygon_on_next_update(this);
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

		update_polygon_on_next_update(this);
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

		update_polygon_on_next_update(this);
	}

	uint
	RectShape::nsegment () const
	{
		return self->nsegment;
	}

	Polygon
	RectShape::update_polygon () const
	{
		return Rays::create_rect(
			frame(),
			self->round_left_top,    self->round_right_top,
			self->round_left_bottom, self->round_right_bottom,
			self->nsegment);
	}

	static bool
	has_rounds (const RectShape& shape)
	{
		const RectShape::Data* self = shape.self.get();
		return
			self->round_left_top     != 0 ||
			self->round_right_top    != 0 ||
			self->round_left_bottom  != 0 ||
			self->round_right_bottom != 0;
	}

	Polygon
	RectShape::get_polygon_for_fixtures () const
	{
		if (self->nsegment <= 0 && has_rounds(*this))
			return Rays::create_rect(frame(), 0, 0, 0, 0, 1);
		else
			return Super::get_polygon_for_fixtures();
	}


	struct EllipseShape::Data
	{

		enum
		{
			DEFAULT_ANGLE_FROM = 0,
			DEFAULT_ANGLE_TO   = 360,

			ANGLE_MAX = DEFAULT_ANGLE_TO - DEFAULT_ANGLE_FROM
		};

		Point hole_size;

		float angle_from = DEFAULT_ANGLE_FROM;
		float angle_to   = DEFAULT_ANGLE_TO;

		uint nsegment = 0;

		bool has_hole () const
		{
			return hole_size.x != 0 && hole_size.y != 0;
		}

		bool has_angle () const
		{
			return
				angle_from != DEFAULT_ANGLE_FROM ||
				angle_to   != DEFAULT_ANGLE_TO;
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
	EllipseShape::set_hole_size (coord width, coord height)
	{
		self->hole_size.reset(width, height);

		update_polygon_on_next_update(this);
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
	EllipseShape::set_angle_from (float degree)
	{
		self->angle_from = degree;

		update_polygon_on_next_update(this);
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

		update_polygon_on_next_update(this);
	}

	float
	EllipseShape::angle_to () const
	{
		return self->angle_to;
	}

	void
	EllipseShape::set_nsegment (uint num_of_segments)
	{
		self->nsegment = num_of_segments;

		update_polygon_on_next_update(this);
	}

	uint
	EllipseShape::nsegment () const
	{
		return self->nsegment;
	}

	static Fixture*
	create_circle_fixture (EllipseShape* shape, const Bounds& frame)
	{
		assert(shape);

		View* view = shape->owner();
		if (!view)
			invalid_state_error(__FILE__, __LINE__);

		float ppm = view->meter2pixel();

		b2CircleShape b2shape;
		b2shape.m_p      = to_b2vec2(frame.center(), ppm);
		b2shape.m_radius = to_b2coord(frame.width / 2, ppm);

		return FixtureBuilder(shape, &b2shape).fixtures();
	}

	Fixture*
	EllipseShape::create_fixtures ()
	{
		Bounds f    = frame();
		bool circle = f.size() != 0 && f.width == f.height;
		if (circle && !self->has_angle() && !self->has_hole() && has_fill(this))
			return create_circle_fixture(this, f);
		else
			return Super::create_fixtures();
	}

	Polygon
	EllipseShape::update_polygon () const
	{
		return Rays::create_ellipse(
			frame(), self->hole_size, self->angle_from, self->angle_to,
			self->nsegment);
	}

	Polygon
	EllipseShape::get_polygon_for_fixtures () const
	{
		if (self->nsegment <= 0)
		{
			return Rays::create_ellipse(
				frame(), self->hole_size, self->angle_from, self->angle_to, 8);
		}
		else
			return Super::get_polygon_for_fixtures();
	}


	WallShape::WallShape (uint positions, coord thickness)
	:	positions(positions), thickness(thickness)
	{
	}

	void
	WallShape::on_draw (DrawEvent* e)
	{
	}

	static Polygon
	make_wall_polygon (
		WallShape::Position pos, const Bounds& frame, coord tickness)
	{
		coord w = frame.width, h = frame.height, t = tickness, tt = tickness * 2;
		switch (pos)
		{
			case WallShape::LEFT:   return Rays::create_rect(-t, -t, t,      h + tt);
			case WallShape::TOP:    return Rays::create_rect(-t, -t, w + tt, t);
			case WallShape::RIGHT:  return Rays::create_rect( w, -t, t,      h + tt);
			case WallShape::BOTTOM: return Rays::create_rect(-t,  h, w + tt, t);
			default: argument_error(__FILE__, __LINE__, "invalid wall position.");
		}
	}

	Polygon
	WallShape::update_polygon () const
	{
		Bounds f = frame();

		Polygon p;
		if (positions & LEFT)   p = p | make_wall_polygon(LEFT,   f, thickness);
		if (positions & TOP)    p = p | make_wall_polygon(TOP,    f, thickness);
		if (positions & RIGHT)  p = p | make_wall_polygon(RIGHT,  f, thickness);
		if (positions & BOTTOM) p = p | make_wall_polygon(BOTTOM, f, thickness);
		return p;
	}


}// Reflex
