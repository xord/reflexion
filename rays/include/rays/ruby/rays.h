// -*- c++ -*-
#pragma once
#ifndef __RAYS_RUBY_RAYS_H__
#define __RAYS_RUBY_RAYS_H__


#include <rucy/rucy.h>
#include <rucy/module.h>
#include <rucy/extension.h>
#include <rays/rays.h>


namespace Rays
{


	Rucy::Module rays_module ();
	// module Rays


}// Rays


RUCY_DECLARE_CONVERT_TO(Rays::CapType)

RUCY_DECLARE_CONVERT_TO(Rays::JoinType)

RUCY_DECLARE_CONVERT_TO(Rays::BlendMode)


#endif//EOH
