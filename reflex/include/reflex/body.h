// -*- c++ -*-
#pragma once
#ifndef __REFLEX_BODY_H__
#define __REFLEX_BODY_H__


#include <rays/point.h>
#include <reflex/defs.h>
#include <reflex/fixture.h>


namespace Reflex
{


	class Body
	{

		public:

			typedef void* Handle;

			typedef FixtureIterator<      Fixture, Fixture>       iterator;

			typedef FixtureIterator<const Fixture, Fixture> const_iterator;

			Fixture add_box (coord width, coord height);

			Fixture add_ellipse (
				coord width, coord height,
				coord radius_min = 0, uint nsegment = 0);

			Fixture add_arc (
				coord width, coord height,
				float angle_from = 0, float angle_to = 360,
				coord radius_min = 0, uint nsegment = 0);

			Fixture add_edge (const Point& begin, const Point& end);

			Fixture add_edge    (const Point* points, size_t size, bool loop = false);

			Fixture add_polygon (const Point* points, size_t size);

			void clear_fixtures ();

			float meter2pixel (float meter = 1) const;

			void set_static (bool state);

			bool  is_static () const;

			void set_dynamic (bool state = true);

			bool  is_dynamic () const;

			Point position () const;

			float angle () const;

			void set_linear_velocity (coord x, coord y);

			void set_linear_velocity (const Point& velocity);

			Point    linear_velocity () const;

			void set_angular_velocity (float velocity);

			float    angular_velocity () const;

			void apply_force (coord x, coord y);

			void apply_force (const Point& force);

			void apply_torque (float torque);

			void apply_linear_impulse (coord x, coord y);

			void apply_linear_impulse (const Point& impulse);

			void apply_angular_impulse (float impulse);

			void set_density (float density);

			float    density () const;

			void set_friction (float friction);

			float    friction () const;

			void set_restitution (float restitution);

			float    restitution () const;

			void set_sensor (bool sensor);

			bool  is_sensor () const;

			void set_category (uint bits);

			uint     category () const;

			void set_collision (uint category_mask);

			uint     collision () const;

			void set_gravity_scale (float scale);

			float    gravity_scale () const;

			      iterator begin ();

			const_iterator begin () const;

			      iterator end ();

			const_iterator end () const;

		protected:

			Body (Handle h, float pixels_per_meter);

		private:

			Handle handle;

			float ppm;

			void set_transform (coord x, coord y, float degree);

			friend class World;

			friend class View;

	};// Body


}// Reflex


#endif//EOH
