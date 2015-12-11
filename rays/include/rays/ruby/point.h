// -*- c++ -*-
#pragma once
#ifndef __RAYS_RUBY_POINT_H__
#define __RAYS_RUBY_POINT_H__


#include <rucy/rucy.h>
#include <rucy/class.h>
#include <rucy/extension.h>
#include <rucy/exception.h>
#include <rays/point.h>


namespace Rays
{


	Rucy::Class point_class ();
	// class Rays::Point


}// Rays


RUCY_DECLARE_VALUE_OR_ARRAY_FROM_TO(Rays::Point)


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Rays::Point> ()
	{
		return Rays::point_class();
	}


}// Rucy


#endif//EOH
