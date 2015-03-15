// -*- c++ -*-
#pragma once
#ifndef __XOT_UTIL_H__
#define __XOT_UTIL_H__


#include <stdint.h>
#include <math.h>
#include <xot/defs.h>
#include <xot/time.h>


namespace Xot
{


	int bit2byte (int bits);

	int byte2bit (int bytes);


	void seed (uint value = (uint) (time() * 1000));

	double random (double max_ = 1);

	double random (double min_, double max_);


	template <typename T>
	inline T deg2rad (T degree)
	{
		return degree / (T) 180 * M_PI;
	}

	template <typename T>
	inline T rad2deg (T radian)
	{
		return radian / M_PI * (T) 180;
	}


	template <typename T>
	inline T clip (T minval, T maxval, T value)
	{
		return value > maxval ? maxval : (value < minval ? minval : value);
	}


	template <typename T>
	inline T* set_pointer_flag (T* pointer, bool flag = true)
	{
		uintptr_t intval = *(uintptr_t*) &pointer;
		if (flag) intval |=  0x1;
		else      intval &= ~0x1;
		return *(T**) &intval;
	}

	template <typename T>
	inline const T* set_pointer_flag (const T* pointer, bool flag = true)
	{
		return set_pointer_flag(const_cast<T*>(pointer), flag);
	}

	template <typename T>
	inline bool get_pointer_flag (const T* pointer)
	{
		uintptr_t intval = *(uintptr_t*) &pointer;
		return intval & 0x1;
	}


}// Xot


#endif//EOH
