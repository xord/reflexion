// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_WORLD_H__
#define __REFLEX_SRC_WORLD_H__


#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <xot/noncopyable.h>
#include <rays/point.h>
#include <rays/painter.h>
#include <reflex/defs.h>


namespace Reflex
{


	class View;

	class Body;

	class DebugDraw;


	class World : public Xot::NonCopyable, private b2ContactListener
	{

		public:

			World (View* owner, float pixels_per_meter = 100);

			~World ();

			void step (float dt);

			Body* create_body (View* owner, const Point& position = 0, float degree = 0);

			void destroy_body (Body* body);

			void resize (coord width, coord height);

			void draw (Painter* painter);

			float meter2pixel (float meter = 1) const;

			void set_gravity (const Point& gravity);

			Point    gravity () const;

			void set_debug (bool state = true);

			bool     debugging () const;

			      Body* wall ();

			const Body* wall () const;

		protected:

			void BeginContact (b2Contact* contact);

			void EndContact   (b2Contact* contact);

		private:

			b2World world;

			float ppm, last_step;

			Body* wall_;

			DebugDraw* debug_draw;

	};// World


	template <typename T>
	inline float32
	to_b2coord (T t, float scale)
	{
		return t / scale;
	}

	template <typename T>
	inline b2Vec2
	to_b2vec2 (T x, T y, float scale)
	{
		return b2Vec2(
			to_b2coord(x, scale),
			to_b2coord(y, scale));
	}

	template <typename VEC>
	inline b2Vec2
	to_b2vec2 (const VEC& v, float scale)
	{
		return b2Vec2(
			to_b2coord(v.x, scale),
			to_b2coord(v.y, scale));
	}

	inline coord
	to_coord (float32 t, float scale)
	{
		return t * scale;
	}

	inline Point
	to_point (const b2Vec2& v, float scale)
	{
		return Point(
			to_coord(v.x, scale),
			to_coord(v.y, scale));
	}


}// Reflex


#endif//EOH
