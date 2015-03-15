// -*- c++ -*-
#pragma once
#ifndef __REFLEX_RUBY_SHAPE_VIEW_H__
#define __REFLEX_RUBY_SHAPE_VIEW_H__


#include <reflex/shape_view.h>
#include <reflex/ruby/view.h>


namespace Reflex
{


	Rucy::Class shape_view_class ();
	// class Reflex::ShapeView

	Rucy::Class rect_shape_class ();
	// class Reflex::RectShape

	Rucy::Class ellipse_shape_class ();
	// class Reflex::EllipseShape

	Rucy::Class arc_shape_class ();
	// class Reflex::ArcShape


	template <typename T>
	class RubyShapeView : public RubyView<T>
	{

		public:

			RUCY_OVERRIDE_BEGIN(RubyView<T>)

			RUCY_OVERRIDE_ID(on_draw_shape)

			RUCY_OVERRIDE_END

			virtual void on_draw_shape (DrawEvent* e)
			{
				RUCY_SYM(on_draw_shape);
				if (RUCY_IS_OVERRIDDEN(on_draw_shape))
					this->value.call(on_draw_shape, Rucy::value(e));
				else
					Super::on_draw_shape(e);
			}

	};// RubyShapeView


}// Reflex


RUCY_DECLARE_WRAPPER_VALUE_FROM_TO(Reflex::ShapeView)

RUCY_DECLARE_WRAPPER_VALUE_FROM_TO(Reflex::RectShape)

RUCY_DECLARE_WRAPPER_VALUE_FROM_TO(Reflex::EllipseShape)

RUCY_DECLARE_WRAPPER_VALUE_FROM_TO(Reflex::ArcShape)


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Reflex::ShapeView> ()
	{
		return Reflex::shape_view_class();
	}

	template <> inline Class
	get_ruby_class<Reflex::RectShape> ()
	{
		return Reflex::rect_shape_class();
	}

	template <> inline Class
	get_ruby_class<Reflex::EllipseShape> ()
	{
		return Reflex::ellipse_shape_class();
	}

	template <> inline Class
	get_ruby_class<Reflex::ArcShape> ()
	{
		return Reflex::arc_shape_class();
	}


}// Rucy


#endif//EOH
