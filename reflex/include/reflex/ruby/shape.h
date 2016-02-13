// -*- c++ -*-
#pragma once
#ifndef __REFLEX_RUBY_SHAPE_H__
#define __REFLEX_RUBY_SHAPE_H__


#include <rucy/rucy.h>
#include <rucy/class.h>
#include <rucy/extension.h>
#include <reflex/shape.h>
#include <reflex/ruby/event.h>


namespace Reflex
{


	Rucy::Class shape_class ();
	// class Reflex::Shape

	Rucy::Class rect_shape_class ();
	// class Reflex::RectShape

	Rucy::Class ellipse_shape_class ();
	// class Reflex::EllipseShape


	template <typename T>
	class RubyShape : public Rucy::ClassWrapper<T>
	{

		public:

			RUCY_OVERRIDE_BEGIN(Rucy::ClassWrapper<T>)

			RUCY_OVERRIDE_ID(on_draw)
			RUCY_OVERRIDE_ID(on_contact)
			RUCY_OVERRIDE_ID(on_contact_begin)
			RUCY_OVERRIDE_ID(on_contact_end)

			RUCY_OVERRIDE_END

			virtual void on_draw (DrawEvent* e)
			{
				RUCY_SYM(on_draw);
				if (RUCY_IS_OVERRIDDEN(on_draw))
					this->value.call(on_draw, Rucy::value(e));
				else
					Super::on_draw(e);
			}

			virtual void on_contact (ContactEvent* e)
			{
				RUCY_SYM(on_contact);
				if (RUCY_IS_OVERRIDDEN(on_contact))
					this->value.call(on_contact, Rucy::value(e));
				else
					Super::on_contact(e);
			}

			virtual void on_contact_begin (ContactEvent* e)
			{
				RUCY_SYM(on_contact_begin);
				if (RUCY_IS_OVERRIDDEN(on_contact_begin))
					this->value.call(on_contact_begin, Rucy::value(e));
				else
					Super::on_contact_begin(e);
			}

			virtual void on_contact_end (ContactEvent* e)
			{
				RUCY_SYM(on_contact_end);
				if (RUCY_IS_OVERRIDDEN(on_contact_end))
					this->value.call(on_contact_end, Rucy::value(e));
				else
					Super::on_contact_end(e);
			}

	};// RubyShape


}// Reflex


RUCY_DECLARE_WRAPPER_VALUE_FROM_TO(Reflex::Shape)

RUCY_DECLARE_WRAPPER_VALUE_FROM_TO(Reflex::RectShape)

RUCY_DECLARE_WRAPPER_VALUE_FROM_TO(Reflex::EllipseShape)


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Reflex::Shape> ()
	{
		return Reflex::shape_class();
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


	inline Value
	value (Reflex::Shape::Ref& ref, Value klass = Reflex::shape_class())
	{
		return value(ref.get(), klass);
	}


}// Rucy


#endif//EOH
