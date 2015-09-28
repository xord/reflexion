// -*- c++ -*-
#pragma once
#ifndef __REFLEX_RUBY_VIEW_H__
#define __REFLEX_RUBY_VIEW_H__


#include <rucy/rucy.h>
#include <rucy/class.h>
#include <rucy/extension.h>
#include <reflex/view.h>
#include <reflex/ruby/event.h>


namespace Reflex
{


	Rucy::Class view_class ();
	// class Reflex::View


	template <typename T>
	class RubyView : public Rucy::ClassWrapper<T>
	{

		public:

			RUCY_OVERRIDE_BEGIN(Rucy::ClassWrapper<T>)

			RUCY_OVERRIDE_ID(content_size)
			RUCY_OVERRIDE_ID(on_update)
			RUCY_OVERRIDE_ID(on_draw)
			RUCY_OVERRIDE_ID(on_move)
			RUCY_OVERRIDE_ID(on_resize)
			RUCY_OVERRIDE_ID(on_rotate)
			RUCY_OVERRIDE_ID(on_scroll)
			RUCY_OVERRIDE_ID(on_focus)
			RUCY_OVERRIDE_ID(on_blur)
			RUCY_OVERRIDE_ID(on_key)
			RUCY_OVERRIDE_ID(on_key_down)
			RUCY_OVERRIDE_ID(on_key_up)
			RUCY_OVERRIDE_ID(on_pointer)
			RUCY_OVERRIDE_ID(on_pointer_down)
			RUCY_OVERRIDE_ID(on_pointer_up)
			RUCY_OVERRIDE_ID(on_pointer_move)
			RUCY_OVERRIDE_ID(on_wheel)
			RUCY_OVERRIDE_ID(on_capture)
			RUCY_OVERRIDE_ID(on_timer)
			RUCY_OVERRIDE_ID(on_contact)
			RUCY_OVERRIDE_ID(on_contact_begin)
			RUCY_OVERRIDE_ID(on_contact_end)

			RUCY_OVERRIDE_END

			virtual Point content_size () const
			{
				RUCY_SYM(content_size);
				if (RUCY_IS_OVERRIDDEN(content_size))
				{
					Rucy::Value ret = this->value.call(content_size);
					return ret.is_nil() ? Super::content_size() : Rucy::to<Point>(ret);
				}
				else
					return Super::content_size();
			}

			virtual void make_body ()
			{
				RUCY_SYM(make_body);
				if (RUCY_IS_OVERRIDABLE())
					this->value.call(make_body);
				else
					Super::make_body();
			}

			virtual void on_attach (Event* e)
			{
				RUCY_SYM(on_attach);
				if (RUCY_IS_OVERRIDABLE())
					this->value.call(on_attach, Rucy::value(e));
				else
					Super::on_attach(e);
			}

			virtual void on_detach (Event* e)
			{
				RUCY_SYM(on_detach);
				if (RUCY_IS_OVERRIDABLE())
					this->value.call(on_detach, Rucy::value(e));
				else
					Super::on_detach(e);
			}

			virtual void on_show (Event* e)
			{
				RUCY_SYM(on_show);
				if (RUCY_IS_OVERRIDABLE())
					this->value.call(on_show, Rucy::value(e));
				else
					Super::on_show(e);
			}

			virtual void on_hide (Event* e)
			{
				RUCY_SYM(on_hide);
				if (RUCY_IS_OVERRIDABLE())
					this->value.call(on_hide, Rucy::value(e));
				else
					Super::on_hide(e);
			}

			virtual void on_update (UpdateEvent* e)
			{
				RUCY_SYM(on_update);
				if (RUCY_IS_OVERRIDDEN(on_update))
					this->value.call(on_update, Rucy::value(e));
				else
					Super::on_update(e);
			}

			virtual void on_draw (DrawEvent* e)
			{
				RUCY_SYM(on_draw);
				if (RUCY_IS_OVERRIDDEN(on_draw))
					this->value.call(on_draw, Rucy::value(e));
				else
					Super::on_draw(e);
			}

			virtual void on_move (FrameEvent* e)
			{
				RUCY_SYM(on_move);
				if (RUCY_IS_OVERRIDDEN(on_move))
					this->value.call(on_move, Rucy::value(e));
				else
					Super::on_move(e);
			}

			virtual void on_resize (FrameEvent* e)
			{
				RUCY_SYM(on_resize);
				if (RUCY_IS_OVERRIDDEN(on_resize))
					this->value.call(on_resize, Rucy::value(e));
				else
					Super::on_resize(e);
			}

			virtual void on_rotate (FrameEvent* e)
			{
				RUCY_SYM(on_rotate);
				if (RUCY_IS_OVERRIDDEN(on_rotate))
					this->value.call(on_rotate, Rucy::value(e));
				else
					Super::on_rotate(e);
			}

			virtual void on_scroll (ScrollEvent* e)
			{
				RUCY_SYM(on_scroll);
				if (RUCY_IS_OVERRIDDEN(on_scroll))
					this->value.call(on_scroll, Rucy::value(e));
				else
					Super::on_scroll(e);
			}

			virtual void on_focus (FocusEvent* e)
			{
				RUCY_SYM(on_focus);
				if (RUCY_IS_OVERRIDDEN(on_focus))
					this->value.call(on_focus, Rucy::value(e));
				else
					Super::on_focus(e);
			}

			virtual void on_blur (FocusEvent* e)
			{
				RUCY_SYM(on_blur);
				if (RUCY_IS_OVERRIDDEN(on_blur))
					this->value.call(on_blur, Rucy::value(e));
				else
					Super::on_blur(e);
			}

			virtual void on_key (KeyEvent* e)
			{
				RUCY_SYM(on_key);
				if (RUCY_IS_OVERRIDDEN(on_key))
					this->value.call(on_key, Rucy::value(e));
				else
					Super::on_key(e);
			}

			virtual void on_key_down (KeyEvent* e)
			{
				RUCY_SYM(on_key_down);
				if (RUCY_IS_OVERRIDDEN(on_key_down))
					this->value.call(on_key_down, Rucy::value(e));
				else
					Super::on_key_down(e);
			}

			virtual void on_key_up (KeyEvent* e)
			{
				RUCY_SYM(on_key_up);
				if (RUCY_IS_OVERRIDDEN(on_key_up))
					this->value.call(on_key_up, Rucy::value(e));
				else
					Super::on_key_up(e);
			}

			virtual void on_pointer (PointerEvent* e)
			{
				RUCY_SYM(on_pointer);
				if (RUCY_IS_OVERRIDDEN(on_pointer))
					this->value.call(on_pointer, Rucy::value(e));
				else
					Super::on_pointer(e);
			}

			virtual void on_pointer_down (PointerEvent* e)
			{
				RUCY_SYM(on_pointer_down);
				if (RUCY_IS_OVERRIDDEN(on_pointer_down))
					this->value.call(on_pointer_down, Rucy::value(e));
				else
					Super::on_pointer_down(e);
			}

			virtual void on_pointer_up (PointerEvent* e)
			{
				RUCY_SYM(on_pointer_up);
				if (RUCY_IS_OVERRIDDEN(on_pointer_up))
					this->value.call(on_pointer_up, Rucy::value(e));
				else
					Super::on_pointer_up(e);
			}

			virtual void on_pointer_move (PointerEvent* e)
			{
				RUCY_SYM(on_pointer_move);
				if (RUCY_IS_OVERRIDDEN(on_pointer_move))
					this->value.call(on_pointer_move, Rucy::value(e));
				else
					Super::on_pointer_move(e);
			}

			virtual void on_wheel (WheelEvent* e)
			{
				RUCY_SYM(on_wheel);
				if (RUCY_IS_OVERRIDDEN(on_wheel))
					this->value.call(on_wheel, Rucy::value(e));
				else
					Super::on_wheel(e);
			}

			virtual void on_capture (CaptureEvent* e)
			{
				RUCY_SYM(on_capture);
				if (RUCY_IS_OVERRIDDEN(on_capture))
					this->value.call(on_capture, Rucy::value(e));
				else
					Super::on_capture(e);
			}

			virtual void on_timer (TimerEvent* e)
			{
				RUCY_SYM(on_timer);
				if (RUCY_IS_OVERRIDDEN(on_timer))
					this->value.call(on_timer, Rucy::value(e));
				else
					Super::on_timer(e);
			}

			virtual void on_contact (ContactEvent* e)
			{
				RUCY_SYM(on_contact);
				if (RUCY_IS_OVERRIDDEN(on_contact))
					this->value.call(on_contact, Rucy::value(e));
				else
					Super::on_contact(e);
			}

			virtual void on_contact_begin (ContactEvent* e)
			{
				RUCY_SYM(on_contact_begin);
				if (RUCY_IS_OVERRIDDEN(on_contact_begin))
					this->value.call(on_contact_begin, Rucy::value(e));
				else
					Super::on_contact_begin(e);
			}

			virtual void on_contact_end (ContactEvent* e)
			{
				RUCY_SYM(on_contact_end);
				if (RUCY_IS_OVERRIDDEN(on_contact_end))
					this->value.call(on_contact_end, Rucy::value(e));
				else
					Super::on_contact_end(e);
			}

	};// RubyView


}// Reflex


RUCY_DECLARE_WRAPPER_VALUE_FROM_TO(Reflex::View)


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Reflex::View> ()
	{
		return Reflex::view_class();
	}

	inline Value
	value (Reflex::View::Ref& ref, Value klass = Reflex::view_class())
	{
		return value(ref.get(), klass);
	}


}// Rucy


#endif//EOH
