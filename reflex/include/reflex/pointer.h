// -*- c++ -*-
#pragma once
#ifndef __REFLEX_POINTER_H__
#define __REFLEX_POINTER_H__


#include <xot/pimpl.h>
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

				MOUSE_LEFT   = 0x1 << 0,

				MOUSE_RIGHT  = 0x1 << 1,

				MOUSE_MIDDLE = 0x1 << 2,

				TOUCH        = 0x1 << 3,

				PEN          = 0x1 << 4,

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
				uint type, Action action, const Point& position,
				uint modifiers, uint click_count, bool drag);

			Pointer (const This& obj);

			Pointer& operator = (const This& obj);

			~Pointer ();

			uint type () const;

			Action action () const;

			const Point& position () const;

			uint modifiers () const;

			uint click_count () const;

			bool is_drag () const;

			//Pointer prev () const;

			operator bool () const;

			bool operator ! () const;

			struct Data;

			Xot::PImpl<Data> self;

	};// Pointer


}// Reflex


#endif//EOH
