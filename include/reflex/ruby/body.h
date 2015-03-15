// -*- c++ -*-
#pragma once
#ifndef __REFLEX_RUBY_BODY_H__
#define __REFLEX_RUBY_BODY_H__


#include <rucy/rucy.h>
#include <rucy/class.h>
#include <rucy/extension.h>
#include <reflex/body.h>


namespace Reflex
{


	Rucy::Class body_class ();
	// class Reflex::Body


}// Reflex


RUCY_DECLARE_VALUE_FROM_TO(Reflex::Body)


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Reflex::Body> ()
	{
		return Reflex::body_class();
	}


}// Rucy


#endif//EOH
