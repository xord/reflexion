// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_BODY_H__
#define __REFLEX_SRC_BODY_H__


#include <xot/noncopyable.h>
#include <xot/pimpl.h>
#include <rays/point.h>
#include <reflex/defs.h>


class b2Body;


namespace Reflex
{


	class Body : public Xot::NonCopyable
	{

		public:

			Body (b2Body* b2body, float pixels_per_meter);

			~Body ();

			void remove_self ();

			void apply_force (coord x, coord y);

			void apply_force (const Point& force);

			void apply_torque (float torque);

			void apply_linear_impulse (coord x, coord y);

			void apply_linear_impulse (const Point& impulse);

			void apply_angular_impulse (float impulse);

			float meter2pixel (float meter = 1) const;

			void set_transform (coord x, coord y, float degree);

			Point position () const;

			float angle () const;

			void set_dynamic (bool dynamic = true);

			bool  is_dynamic () const;

			void set_linear_velocity (coord x, coord y);

			void set_linear_velocity (const Point& velocity);

			Point    linear_velocity () const;

			void set_angular_velocity (float velocity);

			float    angular_velocity () const;

			void set_gravity_scale (float scale);

			float    gravity_scale () const;

			struct Data;

			Xot::PImpl<Data> self;

	};// Body


	void Body_copy_attributes (const Body* from, Body* to);

	Body* Body_create_temporary ();

	bool Body_is_temporary (const Body* body);

	      b2Body* Body_get_b2ptr (      Body* body);

	const b2Body* Body_get_b2ptr (const Body* body);


}// Reflex


#endif//EOH
