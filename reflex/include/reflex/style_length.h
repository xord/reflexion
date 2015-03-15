// -*- c++ -*-
#pragma once
#ifndef __REFLEX_STYLE_LENGTH_H__
#define __REFLEX_STYLE_LENGTH_H__


#include <xot/pimpl.h>
#include <reflex/defs.h>


namespace Reflex
{


	class StyleLength
	{

		public:

			typedef coord Value;

			enum Unit {NONE = 0, PIXEL, PERCENT, UNIT_LAST};

			StyleLength ();

			StyleLength (Value value, Unit unit = PIXEL);

			explicit StyleLength (const char* str);

			StyleLength copy () const;

			void reset (Value value = 0, Unit unit = NONE);

			void reset (const char* str);

			Value value () const;

			Unit unit () const;

			bool get_pixel (coord* pixel, coord parent_size) const;

			String to_s () const;

			operator bool () const;

			bool operator ! () const;

			struct Data;

			Xot::PImpl<Data, true> self;

	};// StyleLength


	class StyleLength2
	{

		public:

			StyleLength2 ();

			StyleLength2 (const StyleLength& all);

			StyleLength2 (const StyleLength& width, const StyleLength& height);

			StyleLength2 copy () const;

			void           set_width (const StyleLength& width);

			const StyleLength& width () const;

			void           set_height (const StyleLength& height);

			const StyleLength& height () const;

			size_t size () const;

			      StyleLength& operator [] (size_t i);

			const StyleLength& operator [] (size_t i) const;

			struct Data;

			Xot::PImpl<Data, true> self;

	};// StyleLength2


	class StyleLength4
	{

		public:

			StyleLength4 ();

			StyleLength4 (const StyleLength& all);

			StyleLength4 (
				const StyleLength& horizontal,
				const StyleLength& vertical);

			StyleLength4 (
				const StyleLength& left,
				const StyleLength& vertical,
				const StyleLength& right);

			StyleLength4 (
				const StyleLength& left,
				const StyleLength& top,
				const StyleLength& right,
				const StyleLength& bottom);

			StyleLength4 copy () const;

			void           set_left (const StyleLength& left);

			const StyleLength& left () const;

			void           set_top (const StyleLength& top);

			const StyleLength& top () const;

			void           set_right (const StyleLength& right);

			const StyleLength& right () const;

			void           set_bottom (const StyleLength& bottom);

			const StyleLength& bottom () const;

			size_t size () const;

			      StyleLength& operator [] (size_t i);

			const StyleLength& operator [] (size_t i) const;

			struct Data;

			Xot::PImpl<Data, true> self;

	};// StyleLength4


}// Reflex


#endif//EOH
