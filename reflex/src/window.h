// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_WINDOW_H__
#define __REFLEX_SRC_WINDOW_H__


#include <reflex/window.h>


namespace Reflex
{


	typedef View* (*CreateRootViewFun) ();


	void  Window_set_create_root_view_fun (CreateRootViewFun fun);

	View* Window_create_root_view ();


}// Reflex


#endif//EOH
