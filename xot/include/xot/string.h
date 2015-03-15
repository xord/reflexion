// -*- c++ -*-
#pragma once
#ifndef __XOT_STRING_H__
#define __XOT_STRING_H__


#include <stdarg.h>
#include <string>


#define XOT_STRINGF(format, result) \
	Xot::String result; \
	do \
	{ \
		if (format) \
		{ \
			va_list args; \
			va_start(args, format); \
			result = Xot::stringf(format, args); \
			va_end(args); \
		} \
	} \
	while (false)


namespace Xot
{


	class String : public std::string
	{

		typedef std::string Super;

		public:

			String ();

			String (const char* str);

			operator const char* () const;

			friend String operator + (const String& lhs, const String& rhs);

			friend String operator + (const String& lhs, const char*   rhs);

			friend String operator + (const char*   lhs, const String& rhs);

	};// String


	String stringf (const char* format, ...);

	String stringf (const char* format, va_list args);

	template <typename T> String to_s (const T& val);


}// Xot


#endif//EOH
