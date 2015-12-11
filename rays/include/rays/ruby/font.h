// -*- c++ -*-
#pragma once
#ifndef __RAYS_RUBY_FONT_H__
#define __RAYS_RUBY_FONT_H__


#include <rucy/rucy.h>
#include <rucy/class.h>
#include <rucy/extension.h>
#include <rays/font.h>


namespace Rays
{


	Rucy::Class font_class ();
	// class Rays::Font


}// Rays


RUCY_DECLARE_VALUE_OR_ARRAY_FROM_TO(Rays::Font)


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Rays::Font> ()
	{
		return Rays::font_class();
	}


}// Rucy


#endif//EOH
