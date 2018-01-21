// -*- c++ -*-
#pragma once
#ifndef __REFLEX_RUBY_FILTER_H__
#define __REFLEX_RUBY_FILTER_H__


#include <rucy/rucy.h>
#include <rucy/class.h>
#include <rucy/extension.h>
#include <reflex/filter.h>


namespace Reflex
{


	Rucy::Class filter_class ();
	// class Reflex::Filter


	template <typename T>
	class RubyFilter : public Rucy::ClassWrapper<T>
	{

		typedef Rucy::ClassWrapper<T> Super;

		public:

			virtual void apply (Painter* painter, const Image& image) const
			{
				RUCY_SYM(apply);
				if (this->is_overridable())
					this->value.call(apply, Rucy::value(painter), Rucy::value(image));
				else
					Super::apply(painter, image);
			}

	};// RubyFilter


}// Reflex


RUCY_DECLARE_WRAPPER_VALUE_FROM_TO(Reflex::Filter)


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Reflex::Filter> ()
	{
		return Reflex::filter_class();
	}

	inline Value
	value (Reflex::Filter::Ref& ref, Value klass = Reflex::filter_class())
	{
		return value(ref.get(), klass);
	}


}// Rucy


#endif//EOH
