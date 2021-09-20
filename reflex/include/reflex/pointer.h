// -*- c++ -*-
#pragma once
#ifndef __REFLEX_POINTER_H__
#define __REFLEX_POINTER_H__


#include <xot/pimpl.h>
#include <xot/util.h>
#include <rays/point.h>
#include <reflex/defs.h>


namespace Reflex
{


	class Pointer
	{

		typedef Pointer This;

		public:

			enum Type
			{

				TYPE_NONE    = 0,

				MOUSE        = Xot::bit(0),

				MOUSE_LEFT   = Xot::bit(1),

				MOUSE_RIGHT  = Xot::bit(2),

				MOUSE_MIDDLE = Xot::bit(3),

				TOUCH        = Xot::bit(4),

				PEN          = Xot::bit(5),

			};// Type

			enum Action
			{

				ACTION_NONE = 0,

				DOWN,

				UP,

				MOVE,

				CANCEL,

				STAY

			};// Action

			Pointer ();

			Pointer (
				uint type, Action action, double time,
				const Point& position, uint modifiers, uint click_count, bool drag);

			Pointer (const This& obj);

			Pointer& operator = (const This& obj);

			~Pointer ();

			uint type () const;

			Action action () const;

			double time () const;

			const Point& position () const;

			uint modifiers () const;

			uint click_count () const;

			bool is_drag () const;

			const Pointer* prev () const;

			operator bool () const;

			bool operator ! () const;

			struct Data;

			Xot::PImpl<Data> self;

	};// Pointer


}// Reflex


#endif//EOH
