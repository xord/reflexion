// -*- c++ -*-
#pragma once
#ifndef __REFLEX_TIMER_H__
#define __REFLEX_TIMER_H__


#include <xot/ref.h>
#include <xot/pimpl.h>
#include <reflex/defs.h>
#include <reflex/selector.h>


namespace Reflex
{


	class View;


	class Timer : public Xot::RefCountable<>
	{

		typedef Timer This;

		public:

			typedef Xot::Ref<This> Ref;

			typedef Selector::      iterator       tag_iterator;

			typedef Selector::const_iterator const_tag_iterator;

			enum {ID_INVALID = 0, ID_FIRST};

			Timer ();

			virtual ~Timer ();

			virtual void fire ();

			virtual void stop ();

			virtual View* owner () const;

			virtual int id () const;

			virtual float interval () const;

			virtual void set_count (int count);

			virtual int      count () const;

			virtual bool is_finished () const;

			virtual void    set_name (const char* name);

			virtual const char* name () const;

			virtual void           add_tag (const char* tag);

			virtual void        remove_tag (const char* tag);

			virtual bool           has_tag (const char* tag) const;

			virtual       tag_iterator tag_begin ();

			virtual const_tag_iterator tag_begin () const;

			virtual       tag_iterator tag_end ();

			virtual const_tag_iterator tag_end () const;

			virtual void        set_selector (const Selector& selector);

			virtual       Selector& selector ();

			virtual const Selector& selector () const;

			virtual operator bool () const;

			virtual bool operator ! () const;

			friend bool operator == (const This& lhs, const This& rhs);

			friend bool operator != (const This& lhs, const This& rhs);

			struct Data;

			Xot::PImpl<Data> self;

	};// Timer


}// Reflex


#endif//EOH
