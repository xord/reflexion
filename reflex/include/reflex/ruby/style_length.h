// -*- c++ -*-
#pragma once
#ifndef __REFLEX_RUBY_STYLE_LENGTH_H__
#define __REFLEX_RUBY_STYLE_LENGTH_H__


#include <rucy/rucy.h>
#include <rucy/class.h>
#include <rucy/extension.h>
#include <reflex/style.h>


namespace Reflex
{


	Rucy::Class style_length_class ();
	// class Reflex::StyleLength

	Rucy::Class style_length2_class ();
	// class Reflex::StyleLength2

	Rucy::Class style_length4_class ();
	// class Reflex::StyleLength4


}// Reflex


RUCY_DECLARE_VALUE_FROM_TO(Reflex::StyleLength)

RUCY_DECLARE_VALUE_FROM_TO(Reflex::StyleLength2)

RUCY_DECLARE_VALUE_FROM_TO(Reflex::StyleLength4)


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Reflex::StyleLength> ()
	{
		return Reflex::style_length_class();
	}

	template <> inline Class
	get_ruby_class<Reflex::StyleLength2> ()
	{
		return Reflex::style_length2_class();
	}

	template <> inline Class
	get_ruby_class<Reflex::StyleLength4> ()
	{
		return Reflex::style_length4_class();
	}


}// Rucy


#endif//EOH
