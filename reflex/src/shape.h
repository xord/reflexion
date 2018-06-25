// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_SHAPE_H__
#define __REFLEX_SRC_SHAPE_H__


#include <xot/util.h>
#include <reflex/shape.h>


class b2Shape;


namespace Reflex
{


	class WallShape : public Shape
	{

		public:

			enum Position
			{

				LEFT   = Xot::bit(0),

				TOP    = Xot::bit(1),

				RIGHT  = Xot::bit(2),

				BOTTOM = Xot::bit(3),

				ALL    = LEFT | TOP | RIGHT | BOTTOM

			};// Position

			enum {DEFAULT_THICKNESS = 100};

			WallShape (uint positions, coord thickness = DEFAULT_THICKNESS);

			virtual void on_draw (DrawEvent* e);

		protected:

			virtual Polygon update_polygon () const;

		private:

			uint positions;

			coord thickness;

	};// WallShape


	bool Shape_set_owner (Shape* shape, View* owner);

	void Shape_update_polygon (Shape* shape, bool force = false);

	void Shape_call_contact_event (Shape* shape, const ContactEvent& event);


}// Reflex


#endif//EOH
