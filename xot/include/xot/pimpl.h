// -*- c++ -*-
#pragma once
#ifndef __XOT_PIMPL_H__
#define __XOT_PIMPL_H__


#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>


namespace Xot
{


	template <typename T>
	class PImpl : public boost::scoped_ptr<T>
	{

		typedef boost::scoped_ptr<T> Super;

		typedef PImpl<T> This;

		public:

			PImpl () : Super(new T) {}

			PImpl (T* p) : Super(p) {}

			PImpl (const This& obj) : Super(new T(*obj)) {}

			This& operator = (const This& obj)
			{
				if (&obj != this) reset(new T(*obj));
				return *this;
			}

	};// PImpl


	template <typename T>
	class PSharedImpl : public boost::shared_ptr<T>
	{

		typedef boost::shared_ptr<T> Super;

		typedef PImpl<T> This;

		public:

			PSharedImpl () : Super(new T) {}

			PSharedImpl (T* p) : Super(p) {}

	};// PSharedImpl


}// Xot


#endif//EOH
