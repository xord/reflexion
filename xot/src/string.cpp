#include "xot/string.h"


#include <stdio.h>
#include <boost/algorithm/string.hpp>
#include <boost/scoped_array.hpp>


namespace Xot
{


	String::String ()
	{
	}

	String::String (const char* str)
	:	Super(str)
	{
	}

	String
	String::upcase () const
	{
		return boost::to_upper_copy(*this);
	}

	String
	String::downcase () const
	{
		return boost::to_lower_copy(*this);
	}

	String
	String::strip () const
	{
		return boost::trim_copy(*this);
	}

	String::operator const char* () const
	{
		return c_str();
	}

	String
	operator + (const String& lhs, const String& rhs)
	{
		String t = lhs;
		t += rhs;
		return t;
	}

	String
	operator + (const String& lhs, const char* rhs)
	{
		String t = lhs;
		t += rhs;
		return t;
	}

	String
	operator + (const char* lhs, const String& rhs)
	{
		String t = lhs;
		t += rhs;
		return t;
	}


	String
	stringf (const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		String ret = stringf(format, args);
		va_end(args);
		return ret;
	}

	String
	stringf (const char* format, va_list args)
	{
		enum {BUFSIZE = 256};
		char stack[BUFSIZE];
		if (vsnprintf(&stack[0], BUFSIZE, format, args) <= BUFSIZE)
			return &stack[0];

		int bufsize = BUFSIZE;// vscprintf(format, args);
		boost::scoped_array<char> heap;
		while (true)
		{
			bufsize *= 2;
			heap.reset(new char[bufsize]);
			if (vsnprintf(&heap[0], bufsize, format, args) <= bufsize)
				return &heap[0];
		}

		return NULL;
	}

	template <> String
	to_s<int> (const int& val)
	{
		return stringf("%d", val);
	}

	template <> String
	to_s<float> (const float& val)
	{
		return stringf("%f", val);
	}

	template <> String
	to_s<double> (const double& val)
	{
		return stringf("%f", val);
	}

	typedef const char* const_char_p;

	template <> String
	to_s<const_char_p> (const const_char_p& val)
	{
		return val;
	}

	template <> String
	to_s<String> (const String& val)
	{
		return val;
	}


}// Xot
