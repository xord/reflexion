// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_FIXTURE_H__
#define __REFLEX_SRC_FIXTURE_H__


#include <memory>
#include <xot/noncopyable.h>
#include <reflex/defs.h>


class b2Fixture;


namespace Reflex
{


	class Fixture : public Xot::NonCopyable
	{

		public:

			Fixture (b2Fixture* b2fixture);

			~Fixture ();

			void set_density (float density);

			float    density () const;

			void set_friction (float friction);

			float    friction () const;

			void set_restitution (float restitution);

			float    restitution () const;

			void set_sensor (bool state);

			bool  is_sensor () const;

			void set_category_bits (uint bits);

			uint     category_bits () const;

			void set_collision_mask (uint mask);

			uint     collision_mask () const;

			      b2Fixture* b2ptr ();

			const b2Fixture* b2ptr () const;

			void       set_next (Fixture* fixture);

			      Fixture* next ();

			const Fixture* next () const;

		private:

			b2Fixture* b2fixture;

			std::unique_ptr<Fixture> pnext;

	};// Fixture


	void     Fixture_copy_attributes (const Fixture* from, Fixture* to);

	Fixture* Fixture_create_temporary ();

	bool     Fixture_is_temporary (const Fixture* fixture);


}// Reflex


#endif//EOH
