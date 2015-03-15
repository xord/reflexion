// -*- c++ -*-
#pragma once
#ifndef __XOT_DEBUG_H__
#define __XOT_DEBUG_H__


namespace Xot
{


#ifdef _DEBUG

	void dout (const char* format, ...);

	void doutln (const char* format, ...);

#else

	inline void dout(...) {}

	inline void doutln(...) {}

#endif


}// Xot


#endif//EOH
