// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SHAPE_H__
#define __REFLEX_SHAPE_H__


#include <xot/ref.h>
#include <xot/pimpl.h>
#include <rays/bounds.h>
#include <reflex/defs.h>
#include <reflex/selector.h>


namespace Reflex
{


	class Event;
	struct DrawEvent;
	struct FrameEvent;
	struct ContactEvent;

	class View;
	class Fixture;


	class Shape : public Xot::RefCountable<>, public HasSelector
	{

		typedef Shape This;

		public:

			typedef Xot::Ref<This> Ref;

			Shape (const char* name = NULL);

			virtual ~Shape ();

			virtual       View* owner ();

			virtual const View* owner () const;

			virtual void set_frame (coord x, coord y, coord width, coord height);

			virtual void set_frame (const Bounds& frame);

			virtual Bounds   frame () const;

			virtual void set_density (float density);

			virtual float    density () const;

			virtual void set_friction (float friction);

			virtual float    friction () const;

			virtual void set_restitution (float restitution);

			virtual float    restitution () const;

			virtual void set_sensor (bool state);

			virtual bool  is_sensor () const;

			virtual void set_category_bits (uint bits);

			virtual uint     category_bits () const;

			virtual void set_collision_mask (uint mask);

			virtual uint     collision_mask () const;

			virtual void on_attach (Event* e);

			virtual void on_detach (Event* e);

			virtual void on_draw   (DrawEvent* e) = 0;

			virtual void on_resize (FrameEvent* e);

			virtual void on_contact       (ContactEvent* e);

			virtual void on_contact_begin (ContactEvent* e);

			virtual void on_contact_end   (ContactEvent* e);

			struct Data;

			Xot::PImpl<Data> self;

		protected:

			virtual Fixture* create_fixtures () = 0;

			virtual SelectorPtr* get_selector_ptr ();

	};// Shape


	class RectShape : public Shape
	{

		typedef Shape Super;

		public:

			RectShape (const char* name = NULL);

			virtual ~RectShape ();

			virtual void set_round (coord round);

			virtual void set_round (
				coord left_top, coord right_top, coord left_bottom, coord right_bottom);

			virtual void set_round_left_top (coord round);

			virtual coord    round_left_top () const;

			virtual void set_round_right_top (coord round);

			virtual coord    round_right_top () const;

			virtual void set_round_left_bottom (coord round);

			virtual coord    round_left_bottom () const;

			virtual void set_round_right_bottom (coord round);

			virtual coord    round_right_bottom () const;

			virtual void set_nsegment (uint num_of_segments);

			virtual uint     nsegment () const;

			virtual void on_draw (DrawEvent* e);

			struct Data;

			Xot::PImpl<Data> self;

		protected:

			virtual Fixture* create_fixtures ();

	};// RectShape


	class EllipseShape : public Shape
	{

		typedef Shape Super;

		public:

			EllipseShape (const char* name = NULL);

			virtual ~EllipseShape ();

			virtual void set_angle_from (float degree);

			virtual float    angle_from () const;

			virtual void set_angle_to (float degree);

			virtual float    angle_to () const;

			virtual void set_radius_min (coord radius);

			virtual coord    radius_min () const;

			virtual void set_nsegment (uint num_of_segments);

			virtual uint     nsegment() const;

			virtual void on_draw (DrawEvent* e);

			struct Data;

			Xot::PImpl<Data> self;

		protected:

			virtual Fixture* create_fixtures ();

	};// EllipseShape


	class LineShape : public Shape
	{

		typedef Shape Super;

		public:

			LineShape (const char* name = NULL);

			virtual ~LineShape ();

			virtual void add (coord x1, coord y1, coord x2, coord y2);

			virtual void add (const Point& p1, const Point& p2);

			virtual void add (const Point* points, size_t size, bool loop = false);

			virtual void on_draw (DrawEvent* e);

			struct Data;

			Xot::PImpl<Data> self;

		protected:

			virtual Fixture* create_fixtures ();

	};// LineShape


	class PolygonShape : public Shape
	{

		typedef Shape Super;

		public:

			PolygonShape (const char* name = NULL);

			virtual ~PolygonShape ();

			virtual void add (const Point* points, size_t size);

			virtual void on_draw (DrawEvent* e);

			struct Data;

			Xot::PImpl<Data> self;

		protected:

			virtual Fixture* create_fixtures ();

	};// PolygonShape


}// Reflex


#endif//EOH
