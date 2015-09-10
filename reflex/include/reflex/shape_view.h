// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SHAPE_VIEW_H__
#define __REFLEX_SHAPE_VIEW_H__


#include <xot/pimpl.h>
#include <reflex/view.h>


namespace Reflex
{


	class ShapeView : public View
	{

		typedef View Super;

		public:

			ShapeView (const char* name = NULL);

			virtual ~ShapeView ();

			virtual Point content_size () const;

	};// ShapeView


	class RectShape : public ShapeView
	{

		typedef ShapeView Super;

		public:

			RectShape (const char* name = NULL);

			virtual ~RectShape ();

			virtual void     set_round (coord round);

			virtual void     set_round (coord width, coord height);

			virtual void     set_round (const Point& round);

			virtual const Point& round () const;

			virtual void on_draw (DrawEvent* e);

			struct Data;

			Xot::PImpl<Data> self;

	};// RectShape


	class EllipseShape : public ShapeView
	{

		typedef ShapeView Super;

		public:

			EllipseShape (const char* name = NULL);

			virtual ~EllipseShape ();

			virtual void set_radius_min (coord radius);

			virtual coord    radius_min () const;

			virtual void set_nsegment (uint num_of_segments);

			virtual uint     nsegment() const;

			virtual void make_body ();

			virtual void on_draw (DrawEvent* e);

			struct Data;

			Xot::PImpl<Data> self;

	};// EllipseShape


	class ArcShape : public EllipseShape
	{

		typedef EllipseShape Super;

		public:

			ArcShape (const char* name = NULL);

			virtual ~ArcShape ();

			virtual void set_angle_from (float degree);

			virtual float    angle_from () const;

			virtual void set_angle_to (float degree);

			virtual float    angle_to () const;

			virtual void make_body ();

			virtual void on_draw (DrawEvent* e);

			struct Data;

			Xot::PImpl<Data> self;

	};// ArcShape


}// Reflex


#endif//EOH
