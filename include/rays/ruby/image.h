// -*- c++ -*-
#pragma once
#ifndef __RAYS_RUBY_IMAGE_H__
#define __RAYS_RUBY_IMAGE_H__


#include <rucy/rucy.h>
#include <rucy/class.h>
#include <rucy/extension.h>
#include <rays/image.h>


namespace Rays
{


	Rucy::Class image_class ();
	// class Rays::Image


}// Rays


RUCY_DECLARE_VALUE_FROM_TO(Rays::Image)


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Rays::Image> ()
	{
		return Rays::image_class();
	}


}// Rucy


#endif//EOH
