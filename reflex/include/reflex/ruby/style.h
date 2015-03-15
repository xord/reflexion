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


	Rucy::Class style_class ();
	// class Reflex::Style


}// Reflex


RUCY_DECLARE_VALUE_FROM_TO(Reflex::Style)


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Reflex::Style> ()
	{
		return Reflex::style_class();
	}


}// Rucy


#endif//EOH
