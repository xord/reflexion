// -*- c++ -*-
#pragma once
#ifndef __REFLEX_RUBY_FIXTURE_H__
#define __REFLEX_RUBY_FIXTURE_H__


#include <rucy/rucy.h>
#include <rucy/class.h>
#include <rucy/extension.h>
#include <reflex/fixture.h>


namespace Reflex
{


	Rucy::Class fixture_class ();
	// class Reflex::Fixture


}// Reflex


RUCY_DECLARE_VALUE_FROM_TO(Reflex::Fixture)


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Reflex::Fixture> ()
	{
		return Reflex::fixture_class();
	}


}// Rucy


#endif//EOH
