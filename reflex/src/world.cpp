#include "world.h"


#include <assert.h>
#include <memory>
#include <Box2D/Common/b2Draw.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <xot/util.h>
#include "reflex/event.h"
#include "reflex/exception.h"
#include "shape.h"
#include "view.h"
#include "body.h"


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
				painter->push_state();
			}

			void end ()
			{
				painter->pop_state();
				painter = NULL;
			}

			void DrawPolygon (
				const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
			{
				assert(painter);

				painter->no_fill();
				painter->set_stroke(color.r, color.g, color.b, color.a * 0.5);

				std::unique_ptr<Point[]> points(new Point[vertexCount]);
				for (int i = 0; i < vertexCount; ++i)
					points[i] = to_point(vertices[i], ppm);

				painter->line(&points[0], vertexCount, true);
			}

			void DrawSolidPolygon (
				const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
			{
				assert(painter);

				painter->set_fill(color.r, color.g, color.b, color.a * 0.5);
				painter->no_stroke();

				std::unique_ptr<Point[]> points(new Point[vertexCount]);
				for (int i = 0; i < vertexCount; ++i)
					points[i] = to_point(vertices[i], ppm);

				painter->line(&points[0], vertexCount, true);
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


	struct World::Data
	{

		b2World b2world;

		float ppm, carried_time;

		std::unique_ptr<DebugDraw> debug_draw;

		Data ()
		:	b2world(b2Vec2(0, 0)), ppm(0), carried_time(0)
		{
		}

	};// World::Data


	World::World (float pixels_per_meter)
	{
		assert(pixels_per_meter > 0);

		self->ppm = pixels_per_meter;

		self->b2world.SetContactListener(this);
	}

	World::~World ()
	{
		self->b2world.SetContactListener(NULL);
	}

	Body*
	World::create_body (const Point& position, float angle)
	{
		if (self->b2world.IsLocked())
			invalid_state_error(__FILE__, __LINE__);

		b2BodyDef def;
		def.position = to_b2vec2(position, self->ppm);
		def.angle    = Xot::deg2rad(angle);

		b2Body* b2body = self->b2world.CreateBody(&def);
		if (!b2body)
			physics_error(__FILE__, __LINE__);

		return new Body(b2body, self->ppm);
	}

	float
	World::meter2pixel (float meter) const
	{
		return meter == 1 ? self->ppm : meter * self->ppm;
	}

	void
	World::set_gravity (const Point& gravity)
	{
		b2Vec2 b2gravity = to_b2vec2(gravity, self->ppm);
		if (b2gravity == self->b2world.GetGravity())
			return;

		self->b2world.SetGravity(b2gravity);
	}

	Point
	World::gravity () const
	{
		return to_point(self->b2world.GetGravity(), self->ppm);
	}

	void
	World::set_debug (bool state)
	{
		if (state == debugging()) return;

		if (state)
		{
			assert(!self->debug_draw);
			self->debug_draw.reset(new DebugDraw(self->ppm));
		}
		else
			self->debug_draw.reset();

		self->b2world.SetDebugDraw(self->debug_draw.get());
	}

	bool
	World::debugging () const
	{
		return !!self->debug_draw;
	}

	void
	World::on_update (float dt)
	{
		static const double DT     = 1. / 60.;
		static const int COUNT_MAX = 8;

		dt                += self->carried_time;
		self->carried_time = fmod(dt, DT);

		int count = (int) (dt / DT);
		if (count > COUNT_MAX)
		{
			count              = COUNT_MAX;
			self->carried_time = 0;
		}

		for (int i = 0; i < count; ++i)
			self->b2world.Step(DT, 8, 4);
	}

	void
	World::on_draw (Painter* painter)
	{
		if (!self->debug_draw) return;

		self->debug_draw->begin(painter);
		self->b2world.DrawDebugData();
		self->debug_draw->end();
	}

	void
	World::BeginContact (b2Contact* contact)
	{
		Shape* s1 = (Shape*) contact->GetFixtureA()->GetUserData();
		if (!s1) return;

		Shape* s2 = (Shape*) contact->GetFixtureB()->GetUserData();
		if (!s2) return;

		if (!View_is_active(*s1->owner()) || !View_is_active(*s2->owner()))
			return;

		Shape_call_contact_event(s1, ContactEvent(ContactEvent::BEGIN, s2));
		Shape_call_contact_event(s2, ContactEvent(ContactEvent::BEGIN, s1));
	}

	void
	World::EndContact (b2Contact* contact)
	{
		Shape* s1 = (Shape*) contact->GetFixtureA()->GetUserData();
		if (!s1) return;

		Shape* s2 = (Shape*) contact->GetFixtureB()->GetUserData();
		if (!s2) return;

		if (!View_is_active(*s1->owner()) || !View_is_active(*s2->owner()))
			return;

		Shape_call_contact_event(s1, ContactEvent(ContactEvent::END, s2));
		Shape_call_contact_event(s2, ContactEvent(ContactEvent::END, s1));
	}


	World*
	World_get_temporary ()
	{
		static World world;
		return &world;
	}

	b2World*
	World_get_b2ptr (World* world)
	{
		return world ? &world->self->b2world : NULL;
	}

	const b2World*
	World_get_b2ptr (const World* world)
	{
		return World_get_b2ptr(const_cast<World*>(world));
	}


}// Reflex
