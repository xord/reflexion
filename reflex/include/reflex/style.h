// -*- c++ -*-
#pragma once
#ifndef __REFLEX_STYLE_H__
#define __REFLEX_STYLE_H__


#include <xot/pimpl.h>
#include <reflex/color.h>
#include <reflex/image.h>
#include <reflex/selector.h>


namespace Reflex
{


	class View;


	class StyleLength
	{

		typedef StyleLength This;

		public:

			typedef coord Value;

			enum Unit {NONE = 0, PIXEL, PERCENT, UNIT_LAST};

			StyleLength ();

			StyleLength (Value value, Unit unit = PIXEL);

			explicit StyleLength (const char* str);

			This copy () const;

			void reset (Value value = 0, Unit unit = NONE);

			void reset (const char* str);

			Value value () const;

			Unit unit () const;

			String to_s () const;

			operator bool () const;

			bool operator ! () const;

			friend bool operator == (const This& lhs, const This& rhs);

			friend bool operator != (const This& lhs, const This& rhs);

			struct Data;

			Xot::PImpl<Data, true> self;

	};// StyleLength


	class Style
	{

		typedef Style This;

		public:

			enum Flow
			{
				FLOW_NONE = 0, FLOW_RIGHT, FLOW_DOWN, FLOW_LEFT, FLOW_UP,
				FLOW_LAST
			};

			typedef Selector::      iterator       tag_iterator;

			typedef Selector::const_iterator const_tag_iterator;

			Style (const char* name = NULL);

			~Style ();

			void    set_name (const char* name);

			const char* name () const;

			void           add_tag (const char* tag);

			void        remove_tag (const char* tag);

			      tag_iterator tag_begin ();

			const_tag_iterator tag_begin () const;

			      tag_iterator tag_end ();

			const_tag_iterator tag_end () const;

			void        set_selector (const Selector& selector);

			      Selector& selector ();

			const Selector& selector () const;

			void set_flow (Flow main, Flow sub = FLOW_NONE);

			void get_flow (Flow* main, Flow* sub) const;

			void            set_width  (const StyleLength& width);

			void            set_height (const StyleLength& height);

			const StyleLength&  width  () const;

			const StyleLength&  height () const;

			void            set_left     (const StyleLength& left);

			void            set_top      (const StyleLength& top);

			void            set_right    (const StyleLength& right);

			void            set_bottom   (const StyleLength& bottom);

			const StyleLength&  left     () const;

			const StyleLength&  top      () const;

			const StyleLength&  right    () const;

			const StyleLength&  bottom   () const;

			void            set_offset_left   (const StyleLength& left);

			void            set_offset_top    (const StyleLength& top);

			void            set_offset_right  (const StyleLength& right);

			void            set_offset_bottom (const StyleLength& bottom);

			const StyleLength&  offset_left   () const;

			const StyleLength&  offset_top    () const;

			const StyleLength&  offset_right  () const;

			const StyleLength&  offset_bottom () const;

			void            set_margin_left   (const StyleLength& left);

			void            set_margin_top    (const StyleLength& top);

			void            set_margin_right  (const StyleLength& right);

			void            set_margin_bottom (const StyleLength& bottom);

			const StyleLength&  margin_left   () const;

			const StyleLength&  margin_top    () const;

			const StyleLength&  margin_right  () const;

			const StyleLength&  margin_bottom () const;

			void            set_padding_left   (const StyleLength& left);

			void            set_padding_top    (const StyleLength& top);

			void            set_padding_right  (const StyleLength& right);

			void            set_padding_bottom (const StyleLength& bottom);

			const StyleLength&  padding_left   () const;

			const StyleLength&  padding_top    () const;

			const StyleLength&  padding_right  () const;

			const StyleLength&  padding_bottom () const;

			void           set_center_x (const StyleLength& x);

			void           set_center_y (const StyleLength& y);

			const StyleLength& center_x () const;

			const StyleLength& center_y () const;

			void     set_fill (const Color& fill);

			const Color& fill () const;

			void     set_stroke (const Color& stroke);

			const Color& stroke () const;

			void     set_image (const Image& image);

			const Image& image () const;

			friend bool operator == (const This& lhs, const This& rhs);

			friend bool operator != (const This& lhs, const This& rhs);

			struct Data;

			Xot::PImpl<Data, true> self;

	};// Style


}// Reflex


#endif//EOH
