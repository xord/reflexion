// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_POINTER_H__
#define __REFLEX_SRC_POINTER_H__


#include <functional>
#include <reflex/pointer.h>


namespace Reflex
{


	enum
	{
		POINTER_ID_INVALID = 0,
		POINTER_ID_FIRST
	};


	void Pointer_update_positions (
		Pointer* pthis, std::function<Point(const Point&)> fun);

	void Pointer_set_id   (Pointer* pthis, uint id);

	void Pointer_set_prev (Pointer* pthis, const Pointer* prev);


}// Reflex


#endif//EOH
