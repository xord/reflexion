// -*- objc -*-
#include "reflex/window.h"


#include "reflex/event.h"
#include "reflex/exception.h"
#include "window_data.h"
#import "native_window.h"


namespace Rays
{
	void set_painter_scale_factor(Painter*, float);
};


namespace Reflex
{


	void set_window (View* view, Window* window);

	View* create_root_view ();

	void call_key_event (View* v, const KeyEvent& e);

	void call_pointer_event (View* v, const PointerEvent& e);

	void call_wheel_event (View* v, const WheelEvent& e);


	void
	set_focus (Window* window, View* view)
	{
		if (!window || !view)
			argument_error(__FILE__, __LINE__);

		View* current = window->self->focus.get();
		if (current == view) return;

		window->self->focus.reset(view);

		FocusEvent e(FocusEvent::BLUR, view, current);

		if (current)
		{
			current->on_blur(&e);
			current->redraw();
		}

		if (view)
		{
			e.type = FocusEvent::FOCUS;
			view->on_focus(&e);
			view->redraw();
		}
	}

	void
	register_capture (View* view)
	{
		if (!view)
			argument_error(__FILE__, __LINE__);

		const Window* window = view->window();
		if (!window)
			invalid_state_error(__FILE__, __LINE__, "this view does not belong to any Window.");

		window->self->capturing_views[view] = true;
	}

	void
	unregister_capture (View* view)
	{
		if (!view)
			argument_error(__FILE__, __LINE__);

		const Window* window = view->window();
		if (!window) return;

		CapturingViews::iterator it = window->self->capturing_views.find(view);
		if (it == window->self->capturing_views.end()) return;

		it->second = false;
	}

	void
	cleanup_capturing_views (Window* window)
	{
		CapturingViews::iterator end = window->self->capturing_views.end();
		for (CapturingViews::iterator it = window->self->capturing_views.begin(); it != end;)
		{
			CapturingViews::iterator t = it++;
			if (!t->second) window->self->capturing_views.erase(t);
		}
	}


	Window::Window ()
	{
		[[[NativeWindow alloc] init] bind: this];

		self->root.reset(create_root_view());
		self->root->set_name("ROOT");
		set_window(self->root.get(), this);

		Rays::set_painter_scale_factor(&self->painter, UIScreen.mainScreen.scale);

		self->painter.canvas(0, 0, 1, 1);
	}

	Window::~Window ()
	{
		//close();

		set_window(self->root.get(), NULL);
	}

	void
	Window::show ()
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		int new_count = self->hide_count - 1;
		if (new_count == 0)
		{
			Event e;
			on_show(&e);
			if (e.is_blocked()) return;

			[self->native makeKeyAndVisible];
		}

		self->hide_count = new_count;
	}

	void
	Window::hide ()
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		int new_count = self->hide_count + 1;
		if (new_count == 1)
		{
			Event e;
			on_hide(&e);
			if (e.is_blocked()) return;

			not_implemented_error(__FILE__, __LINE__);
		}

		self->hide_count = new_count;
	}

	void
	Window::close ()
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		Event e;
		on_close(&e);
		if (e.is_blocked()) return;

		not_implemented_error(__FILE__, __LINE__);
	}

	void
	Window::redraw ()
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		self->redraw = true;
	}

	void
	Window::set_title (const char* title)
	{
		if (!title)
			argument_error(__FILE__, __LINE__);

		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		self->title = title;
	}

	const char*
	Window::title () const
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		return self->title.c_str();
	}

	void
	Window::set_frame (coord x, coord y, coord width, coord height)
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		[self->native frameChanged];
	}

	void
	Window::set_frame (const Bounds& bounds)
	{
		set_frame(bounds.x, bounds.y, bounds.width, bounds.height);
	}

	Bounds
	Window::frame () const
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		CGRect rect = self->native.frame;
		return Bounds(
			rect.origin.x,
			rect.origin.y,
			rect.size.width,
			rect.size.height);
	}

	bool
	Window::hidden () const
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		return self->hide_count > 0;
	}

	View*
	Window::root ()
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		return self->root.get();
	}

	View*
	Window::focus ()
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		return self->focus.get();
	}

	Painter*
	Window::painter ()
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		return &self->painter;
	}

	void
	Window::on_key (KeyEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);

		switch (e->type)
		{
			case KeyEvent::DOWN: on_key_down(e); break;
			case KeyEvent::UP:   on_key_up(e);   break;
			case KeyEvent::NONE: break;
		}

		CapturingViews::iterator end = self->capturing_views.end();
		for (CapturingViews::iterator it = self->capturing_views.begin(); it != end; ++it)
		{
			KeyEvent event = *e;
			event.capture = true;
			call_key_event(const_cast<View*>(it->first.get()), event);
		}

		if (self->focus)
			call_key_event(self->focus.get(), *e);

		cleanup_capturing_views(this);
	}

	void
	Window::on_pointer (PointerEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);

		switch (e->type)
		{
			case PointerEvent::DOWN: on_pointer_down(e); break;
			case PointerEvent::UP:   on_pointer_up(e);   break;
			case PointerEvent::MOVE: on_pointer_move(e); break;
			case PointerEvent::NONE: break;
		}

		CapturingViews::iterator end = self->capturing_views.end();
		for (CapturingViews::iterator it = self->capturing_views.begin(); it != end; ++it)
		{
			PointerEvent event = *e;
			event.capture = true;
			for (size_t i = 0; i < event.size; ++i)
				event[i] = it->first.get()->from_window(event[i]);
			call_pointer_event(const_cast<View*>(it->first.get()), event);
		}

		call_pointer_event(root(), *e);

		cleanup_capturing_views(this);
	}

	void
	Window::on_wheel (WheelEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);

		call_wheel_event(root(), *e);
	}

	Window::operator bool () const
	{
		return self && *self;
	}


}// Reflex
