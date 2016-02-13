// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_SHAPE_H__
#define __REFLEX_SRC_SHAPE_H__


#include <reflex/shape.h>


class b2Shape;


namespace Reflex
{


	class FixtureBuilder
	{

		public:

			FixtureBuilder (Shape* shape, const b2Shape* head = NULL);

			void add (const b2Shape* b2shape);

			Fixture* fixtures () const;

		private:

			Shape* shape;

			Fixture *head, *tail;

			void append (Fixture* fixture);

	};// FixtureBuilder


	bool Shape_set_owner (Shape* shape, View* owner);

	void Shape_update_fixtures (Shape* shape, bool force = false);

	void Shape_call_contact_event (Shape* shape, const ContactEvent& event);


}// Reflex


#endif//EOH
