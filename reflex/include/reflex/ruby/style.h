// -*- c++ -*-
#pragma once
#ifndef __REFLEX_RUBY_STYLE_H__
#define __REFLEX_RUBY_STYLE_H__


#include <rucy/rucy.h>
#include <rucy/class.h>
#include <rucy/extension.h>
#include <reflex/style.h>


namespace Reflex
{


	Rucy::Class style_length_class ();
	// class Reflex::StyleLength

	Rucy::Class style_class ();
	// class Reflex::Style


}// Reflex


RUCY_DECLARE_VALUE_OR_ARRAY_FROM_TO(Reflex::StyleLength)

RUCY_DECLARE_VALUE_FROM_TO(Reflex::Style)


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Reflex::StyleLength> ()
	{
		return Reflex::style_length_class();
	}

	template <> inline Class
	get_ruby_class<Reflex::Style> ()
	{
		return Reflex::style_class();
	}


}// Rucy


#endif//EOH
