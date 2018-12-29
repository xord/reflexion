// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_IOS_APPLICATION_DATA_H__
#define __REFLEX_SRC_IOS_APPLICATION_DATA_H__


#include <reflex/application.h>


@class ReflexAppDelegate;


namespace Reflex
{


	struct Application::Data
	{

		ReflexAppDelegate* delegate;

		String name;

		Data ()
		:	delegate(nil)
		{
		}

		operator bool () const
		{
			return delegate;
		}

		bool operator ! () const
		{
			return !operator bool();
		}

	};// Application::Data


}// Reflex


#endif//EOH
