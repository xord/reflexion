// -*- c++ -*-
#pragma once
#ifndef __XOT_PIMPL_H__
#define __XOT_PIMPL_H__


#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>


namespace Xot
{


	template <typename T, bool SHARED = false> class PImpl;


	template <typename T>
	class PImpl<T, false> : public boost::scoped_ptr<T>
	{

		typedef boost::scoped_ptr<T> Super;

		typedef PImpl<T, false> This;

		public:

			PImpl () : Super(new T) {}

			PImpl (T* p) : Super(p) {}

			PImpl (const This& obj) : Super(new T(*obj)) {}

			This& operator = (const This& obj)
			{
				if (&obj != this) reset(new T(*obj));
				return *this;
			}

			bool shared () const {return false;}

	};// PImpl


	template <typename T>
	class PImpl<T, true> : public boost::shared_ptr<T>
	{

		typedef boost::shared_ptr<T> Super;

		typedef PImpl<T, true> This;

		public:

			PImpl () : Super(new T) {}

			PImpl (T* p) : Super(p) {}

			bool shared () const {return true;}

	};// PImpl


}// Xot


#endif//EOH
