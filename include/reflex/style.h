// -*- c++ -*-
#pragma once
#ifndef __REFLEX_STYLE_H__
#define __REFLEX_STYLE_H__


#include <xot/ref.h>
#include <reflex/color.h>
#include <reflex/image.h>
#include <reflex/selector.h>
#include <reflex/style_length.h>


namespace Reflex
{


	class Style
	{

		typedef Style This;

		public:

			enum Flow {FLOW_NONE = 0, FLOW_DOWN, FLOW_RIGHT, FLOW_UP, FLOW_LEFT, FLOW_LAST};

			typedef Selector::      iterator       tag_iterator;

			typedef Selector::const_iterator const_tag_iterator;

			Style (const char* name = NULL);

			Style (const This& obj);

			Style& operator = (const This& obj);

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

			void            set_size (const StyleLength2& size);

			void            set_width  (const StyleLength& width);

			void            set_height (const StyleLength& height);

			const StyleLength2& size () const;

			void            set_position (const StyleLength4& position);

			void            set_left   (const StyleLength& left);

			void            set_top    (const StyleLength& top);

			void            set_right  (const StyleLength& right);

			void            set_bottom (const StyleLength& bottom);

			const StyleLength4& position () const;

			void            set_offset (const StyleLength4& offset);

			void            set_offset_left   (const StyleLength& left);

			void            set_offset_top    (const StyleLength& top);

			void            set_offset_right  (const StyleLength& right);

			void            set_offset_bottom (const StyleLength& bottom);

			const StyleLength4& offset () const;

			void            set_margin (const StyleLength4& margin);

			void            set_margin_left   (const StyleLength& left);

			void            set_margin_top    (const StyleLength& top);

			void            set_margin_right  (const StyleLength& right);

			void            set_margin_bottom (const StyleLength& bottom);

			const StyleLength4& margin () const;

			void            set_padding (const StyleLength4& padding);

			void            set_padding_left   (const StyleLength& left);

			void            set_padding_top    (const StyleLength& top);

			void            set_padding_right  (const StyleLength& right);

			void            set_padding_bottom (const StyleLength& bottom);

			const StyleLength4& padding () const;

			void     set_background_color (const Color& value);

			const Color& background_color () const;

			void     set_background_image (const Image& value);

			const Image& background_image () const;

			friend bool operator == (const This& lhs, const This& rhs);

			friend bool operator != (const This& lhs, const This& rhs);

			struct Data;

			Xot::Ref<Data> ref;

	};// Style


}// Reflex


#endif//EOH
