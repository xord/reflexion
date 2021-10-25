#include "world.h"


#include <assert.h>
#include <memory>
#include <Box2D/Common/b2Draw.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include "reflex/event.h"
#include "reflex/exception.h"
#include "shape.h"
#include "view.h"
#include "body.h"
#include "fixture.h"


namespace Reflex
{


	static constexpr double DELTA_TIME = 1. / 60.;


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

		float ppm, time_scale;

		std::unique_ptr<DebugDraw> debug_draw;

		Data ()
		:	b2world(b2Vec2(0, 0)), ppm(0), time_scale(1)
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

	void
	World::update (float duration)
	{
		float dt = DELTA_TIME * self->time_scale;
		if (dt <= 0) return;

		int count = (int) (duration / DELTA_TIME);
		if (count < 1) count = 1;

		for (int i = 0; i < count; ++i)
			self->b2world.Step(dt, 8, 4);
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
	World::set_time_scale (float scale)
	{
		self->time_scale = scale;
	}

	float
	World::time_scale () const
	{
		return self->time_scale;
	}

	void
	World::set_debug (bool state)
	{
		if (state == debug()) return;

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
	World::debug () const
	{
		return !!self->debug_draw;
	}

	void
	World::on_update (float dt)
	{
		update(DELTA_TIME);
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
