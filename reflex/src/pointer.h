// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_POINTER_H__
#define __REFLEX_SRC_POINTER_H__


#include <functional>
#include <reflex/pointer.h>


namespace Reflex
{


	void Pointer_update_positions (
		Pointer* pthis, std::function<void(Point*)> fun);

	void Pointer_set_id   (Pointer* pthis, Pointer::ID id);

	void Pointer_set_prev (Pointer* pthis, const Pointer* prev);


}// Reflex


#endif//EOH
