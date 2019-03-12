// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_OSX_APPLICATION_DATA_H__
#define __REFLEX_SRC_OSX_APPLICATION_DATA_H__


#include <reflex/application.h>


@class ReflexAppDelegate;


namespace Reflex
{


	struct Application::Data
	{

		ReflexAppDelegate* delegate = nil;

		String name;

	};// Application::Data


}// Reflex


#endif//EOH
