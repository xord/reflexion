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


RUCY_DECLARE_VALUE_OR_ARRAY_TO(Rays::CapType)

RUCY_DECLARE_VALUE_OR_ARRAY_TO(Rays::JoinType)


#endif//EOH
