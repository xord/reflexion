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

		public:

			RUCY_OVERRIDE_BEGIN(Rucy::ClassWrapper<T>)

			RUCY_OVERRIDE_ID(apply)

			RUCY_OVERRIDE_END

			virtual void apply (Painter* painter, const Image& image) const
			{
				RUCY_SYM(apply);
				if (RUCY_IS_OVERRIDDEN(apply))
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
