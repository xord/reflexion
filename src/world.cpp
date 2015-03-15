#include "world.h"


#include <assert.h>
#include <boost/scoped_array.hpp>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Common/b2Draw.h>
#include "xot/util.h"
#include "reflex/body.h"
#include "reflex/view.h"
#include "reflex/event.h"
#include "reflex/exception.h"


namespace Reflex
{


	class DebugDraw : public b2Draw
	{

		public:

			DebugDraw (float ppm)
			:	ppm(ppm), painter(NULL)
			{
				SetFlags(
					e_shapeBit |
					e_jointBit |
					//e_aabbBit  |
					e_pairBit  |
					e_centerOfMassBit);
			}

			void begin (Painter* painter)
			{
				this->painter = painter;
				painter->push_attr();
			}

			void end ()
			{
				painter->pop_attr();
				painter = NULL;
			}

			void DrawPolygon (
				const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
			{
				assert(painter);

				painter->no_fill();
				painter->set_stroke(color.r, color.g, color.b, color.a * 0.5);

				boost::scoped_array<Coord2> points(new Coord2[vertexCount]);
				for (int i = 0; i < vertexCount; ++i)
					points[i] = to_coord2(vertices[i], ppm);

				painter->polygon(&points[0], vertexCount);
			}

			void DrawSolidPolygon (
				const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
			{
				assert(painter);

				painter->set_fill(color.r, color.g, color.b, color.a * 0.5);
				painter->no_stroke();

				boost::scoped_array<Coord2> points(new Coord2[vertexCount]);
				for (int i = 0; i < vertexCount; ++i)
					points[i] = to_coord2(vertices[i], ppm);

				painter->polygon(&points[0], vertexCount);
			}

			void DrawCircle (
				const b2Vec2& center, float32 radius, const b2Color& color)
			{
				assert(painter);

				painter->no_fill();
				painter->set_stroke(color.r, color.g, color.b, color.a * 0.5);
				painter->ellipse(to_point(center, ppm), to_coord(radius, ppm));
			}

			void DrawSolidCircle (
				const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
			{
				assert(painter);

				painter->set_fill(color.r, color.g, color.b, color.a * 0.5);
				painter->no_stroke();
				painter->ellipse(to_point(center, ppm), to_coord(radius, ppm));
			}

			void DrawSegment (
				const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
			{
				assert(painter);

				painter->no_fill();
				painter->set_stroke(color.r, color.g, color.b, color.a * 0.5);
				painter->line(to_point(p1, ppm), to_point(p2, ppm));
			}

			void DrawTransform (const b2Transform& transform)
			{
				assert(painter);
			}

		private:

			float ppm;

			Painter* painter;

	};// DebugDraw


	World::World (View* owner, float pixels_per_meter)
	:	world(b2Vec2(0, 0)), ppm(pixels_per_meter), last_step(0), wall_(NULL),
		debug_draw(NULL)
	{
		world.SetContactListener(this);
		wall_ = create_body(owner);
	}

	World::~World ()
	{
		delete debug_draw;
	}

	void
	World::step (float dt)
	{
		static const double DT = 1. / 60.;

		dt       += last_step;
		last_step = fmod(dt, DT);

		int count = (int) (dt / DT);
		for (int i = 0; i < count; ++i)
			world.Step(DT, 8, 4);
	}

	Body*
	World::create_body (View* owner, const Point& position, float degree)
	{
		if (!owner)
			argument_error(__FILE__, __LINE__);

		if (world.IsLocked())
			invalid_state_error(__FILE__, __LINE__);

		b2BodyDef def;
		def.position = to_b2vec2(position, ppm);
		def.angle    = Xot::deg2rad(degree);
		b2Body* body = world.CreateBody(&def);
		body->SetUserData(owner);
		return new Body(body, ppm);
	}

	void
	World::destroy_body (Body* body)
	{
		if (!body)
			argument_error(__FILE__, __LINE__);

		b2Body* b = (b2Body*) body->handle;
		if (!b || world.IsLocked())
			invalid_state_error(__FILE__, __LINE__);

		world.DestroyBody(b);
	}

	void
	World::resize (coord width, coord height)
	{
		wall_->clear_fixtures();

		const Point points[] = {
			Point(0,     0),
			Point(0,     height),
			Point(width, height),
			Point(width, 0),
		};
		wall_->add_edge(points, 4, true);
	}

	void
	World::draw (Painter* painter)
	{
		if (!debug_draw) return;

		debug_draw->begin(painter);
		world.DrawDebugData();
		debug_draw->end();
	}

	float
	World::meter2pixel (float meter) const
	{
		return meter * ppm;
	}

	void
	World::set_gravity (const Point& gravity)
	{
		world.SetGravity(to_b2vec2(gravity, ppm));
	}

	Point
	World::gravity () const
	{
		return to_point(world.GetGravity(), ppm);
	}

	Body*
	World::wall ()
	{
		return wall_;
	}

	const Body*
	World::wall () const
	{
		return const_cast<World*>(this)->wall();
	}

	void
	World::set_debug (bool state)
	{
		if (state == debugging()) return;

		if (!debug_draw)
			debug_draw = new DebugDraw(ppm);
		else
		{
			delete debug_draw;
			debug_draw = NULL;
		}

		world.SetDebugDraw(debug_draw);
	}

	bool
	World::debugging () const
	{
		return debug_draw;
	}

	void
	World::BeginContact (b2Contact* contact)
	{
		View* v1 = (View*) contact->GetFixtureA()->GetBody()->GetUserData();
		if (!v1) return;

		View* v2 = (View*) contact->GetFixtureB()->GetBody()->GetUserData();
		if (!v2) return;

		ContactEvent e1(ContactEvent::BEGIN, v2);
		v1->on_contact(&e1);

		ContactEvent e2(ContactEvent::BEGIN, v1);
		v2->on_contact(&e2);
	}

	void
	World::EndContact (b2Contact* contact)
	{
		View* v1 = (View*) contact->GetFixtureA()->GetBody()->GetUserData();
		if (!v1) return;

		View* v2 = (View*) contact->GetFixtureB()->GetBody()->GetUserData();
		if (!v2) return;

		ContactEvent e1(ContactEvent::END, v2);
		v1->on_contact(&e1);

		ContactEvent e2(ContactEvent::END, v1);
		v2->on_contact(&e2);
	}


}// Reflex
