// -*- c++ -*-
#pragma once
#ifndef __REFLEX_FIXTURE_H__
#define __REFLEX_FIXTURE_H__


#include <iterator>


namespace Reflex
{


	class Fixture
	{

		typedef Fixture This;

		public:

			typedef void* Handle;

			Fixture (Handle h);

			This& operator = (Handle h);

			void set_density (float density);

			float    density () const;

			void set_friction (float friction);

			float    friction () const;

			void set_restitution (float restitution);

			float    restitution () const;

			Handle next () const;

			operator bool () const;

			bool operator ! () const;

			friend bool operator == (const This& lhs, const This& rhs);

			friend bool operator != (const This& lhs, const This& rhs);

		private:

			Handle handle;

	};// Fixture


	template <typename FIXTURE, typename RAW_FIXTURE>
	class FixtureIterator :
		public std::iterator<std::forward_iterator_tag, FIXTURE>
	{

		typedef FixtureIterator This;

		public:

			FixtureIterator (const FIXTURE& fixture)
			:	fixture(fixture)
			{
			}

			FIXTURE* operator -> () {return &fixture;}

			Fixture& operator * () {return fixture;}

			This& operator ++ ()
			{
				fixture = fixture.next();
				return *this;
			}

			This operator ++ (int)
			{
				This tmp(fixture);
				fixture = fixture.next();
				return tmp;
			}

			operator bool () const
			{
				return !!fixture;
			}

			bool operator ! () const
			{
				return !operator bool();
			}

			friend bool operator == (const This& lhs, const This& rhs)
			{
				return lhs.fixture == rhs.fixture;
			}

			friend bool operator != (const This& lhs, const This& rhs)
			{
				return !(lhs == rhs);
			}

		private:

			RAW_FIXTURE fixture;

	};// FixtureIterator


}// Reflex


#endif//EOH
