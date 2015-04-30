// -*- c++ -*-
#pragma once
#ifndef __REFLEX_VIEW_H__
#define __REFLEX_VIEW_H__


#include <vector>
#include <xot/ref.h>
#include <xot/pimpl.h>
#include <reflex/point.h>
#include <reflex/bounds.h>
#include <reflex/painter.h>
#include <reflex/style.h>
#include <reflex/event.h>


//#define REFLEX_VIEW_REF_DEBUG 1


namespace Reflex
{


	class Window;

	class Body;


	class View : public Xot::RefCountable<>
	{

		typedef View This;

		public:

			typedef Xot::Ref<This> Ref;

			typedef std::vector<Ref>   ChildList;

			typedef std::vector<Style> StyleList;

			typedef ChildList::      iterator       child_iterator;

			typedef ChildList::const_iterator const_child_iterator;

			typedef StyleList::      iterator       style_iterator;

			typedef StyleList::const_iterator const_style_iterator;

			typedef Selector::       iterator         tag_iterator;

			typedef Selector:: const_iterator   const_tag_iterator;

			enum Capture
			{

				CAPTURE_NONE      = 0,

				CAPTURE_KEY       = 0x1 << 0,

				CAPTURE_POINTER   = 0x1 << 1,

				CAPTURE_ALL       = CAPTURE_KEY | CAPTURE_POINTER,

			};// Capture

			View (const char* name = NULL);

			virtual void show ();

			virtual void hide ();

			virtual bool hidden () const;

			virtual void redraw ();

			virtual void             add_child (View* child);

			virtual void          remove_child (View* child);

			virtual void            find_children (
				ChildList* result, const Selector& selector, bool recursive = false) const;

			virtual       child_iterator child_begin ();

			virtual const_child_iterator child_begin () const;

			virtual       child_iterator child_end ();

			virtual const_child_iterator child_end () const;

			virtual       Style&         style ();

			virtual const Style&         style () const;

			virtual void             add_style (const Style& style);

			virtual void          remove_style (const Style& style);

			virtual       Style*     get_style (const Selector& selector);

			virtual const Style*     get_style (const Selector& selector) const;

			virtual void            find_styles (
				StyleList* result, const Selector& selector, bool recursive = false) const;

			virtual       style_iterator style_begin ();

			virtual const_style_iterator style_begin () const;

			virtual       style_iterator style_end ();

			virtual const_style_iterator style_end () const;

			virtual void focus (bool state = true);

			virtual void blur ();

			virtual bool has_focus () const;

			virtual void resize_to_fit ();

			virtual Point content_size () const;

			virtual void make_body ();

			virtual void clear_body ();

			virtual void    set_name (const char* name);

			virtual const char* name () const;

			virtual void           add_tag (const char* tag);

			virtual void        remove_tag (const char* tag);

			virtual       tag_iterator tag_begin ();

			virtual const_tag_iterator tag_begin () const;

			virtual       tag_iterator tag_end ();

			virtual const_tag_iterator tag_end () const;

			virtual void        set_selector (const Selector& selector);

			virtual       Selector& selector ();

			virtual const Selector& selector () const;

			virtual void      set_frame (coord x, coord y, coord width, coord height);

			virtual void      set_frame (const Bounds& bounds);

			virtual const Bounds& frame () const;

			virtual void set_zoom (float zoom);

			virtual float    zoom () const;

			virtual float angle () const;

			virtual void         scroll_to (coord x, coord y, coord z = 0);

			virtual void         scroll_to (const Point& scroll);

			virtual void         scroll_by (coord x, coord y, coord z = 0);

			virtual void         scroll_by (const Point& dscroll);

			virtual const Point& scroll () const;

			virtual void set_capture (uint types);

			virtual uint     capture () const;

			virtual       View* parent ();

			virtual const View* parent () const;

			virtual       Window* window ();

			virtual const Window* window () const;

			virtual       Body* body ();

			virtual const Body* body () const;

			virtual float meter2pixel (float meter = 1, bool create_world = true);

			virtual float meter2pixel (float meter = 1) const;

			virtual void set_gravity (coord x, coord y);

			virtual void set_gravity (const Point& vector);

			virtual Point    gravity () const;

			virtual       Body* wall ();

			virtual const Body* wall () const;

			virtual void set_debug (bool state);

			virtual bool     debugging () const;

			virtual Point from_parent (const Point& point) const;

			virtual Point   to_parent (const Point& point) const;

			virtual Point from_window (const Point& point) const;

			virtual Point   to_window (const Point& point) const;

			virtual Point from_screen (const Point& point) const;

			virtual Point   to_screen (const Point& point) const;

			virtual void on_attach (Event* e);

			virtual void on_detach (Event* e);

			virtual void on_show (Event* e);

			virtual void on_hide (Event* e);

			virtual void on_update (UpdateEvent* e);

			virtual void on_draw   (DrawEvent* e);

			virtual void on_move   (FrameEvent* e);

			virtual void on_resize (FrameEvent* e);

			virtual void on_rotate (FrameEvent* e);

			virtual void on_scroll (ScrollEvent* e);

			virtual void on_focus (FocusEvent* e);

			virtual void on_blur (FocusEvent* e);

			virtual void on_key      (KeyEvent* e);

			virtual void on_key_down (KeyEvent* e);

			virtual void on_key_up   (KeyEvent* e);

			virtual void on_pointer      (PointerEvent* e);

			virtual void on_pointer_down (PointerEvent* e);

			virtual void on_pointer_up   (PointerEvent* e);

			virtual void on_pointer_move (PointerEvent* e);

			virtual void on_wheel (WheelEvent* e);

			virtual void on_capture (CaptureEvent* e);

			virtual void on_contact       (ContactEvent* e);

			virtual void on_contact_begin (ContactEvent* e);

			virtual void on_contact_end   (ContactEvent* e);

			virtual operator bool () const;

			virtual bool operator ! () const;

			struct Data;

			Xot::PImpl<Data> self;

			#ifdef REFLEX_VIEW_REF_DEBUG
				virtual void retain (void* data = NULL) const
				{
					RefCountable::retain(data);
					String spaces = "";
					for (int i = 0; i < refc_count(); ++i) spaces += " ";
					printf("(%d)%s-> %s\n", refc_count(), spaces.c_str(), name());
				}

				virtual void release (void* data = NULL) const
				{
					RefCountable::release(data);
					String spaces = "";
					for (int i = 0; i < refc_count(); ++i) spaces += " ";
					printf("(%d)%s<- %s\n", refc_count(), spaces.c_str(), name());
				}
			#endif// REFLEX_VIEW_REF_DEBUG

		protected:

			virtual ~View ();

	};// View


}// Reflex


#endif//EOH
