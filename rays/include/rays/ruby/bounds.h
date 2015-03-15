// -*- c++ -*-
#pragma once
#ifndef __RAYS_RUBY_BOUNDS_H__
#define __RAYS_RUBY_BOUNDS_H__


#include <rucy/rucy.h>
#include <rucy/class.h>
#include <rucy/extension.h>
#include <rucy/exception.h>
#include <rays/bounds.h>
#include <rays/ruby/point.h>


namespace Rays
{


	Rucy::Class bounds_class ();
	// class Rays::Bounds


}// Rays


RUCY_DECLARE_VALUE_FROM_TO(Rays::Bounds)


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Rays::Bounds> ()
	{
		return Rays::bounds_class();
	}


}// Rucy


#endif//EOH
