#include "window.h"


#include "reflex/exception.h"
#include "view.h"
#include "event.h"


namespace Reflex
{


	void
	Window_set_focus (Window* window, View* view)
	{
		if (!window || !view)
			argument_error(__FILE__, __LINE__);

		View* current = window->self->focus.get();
		if (current == view) return;

		window->self->focus.reset(view);

		FocusEvent e(FocusEvent::BLUR, view, current);

		if (current)
		{
			current->on_focus(&e);
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
	Window_call_draw_event (Window* window, DrawEvent* event)
	{
		if (!window || !event)
			argument_error(__FILE__, __LINE__);

		Painter* painter = window->painter();
		if (!painter)
			Xot::invalid_state_error(__FILE__, __LINE__);

		Rays::Bounds frame = window->frame();

		event->painter = painter;
		event->bounds.reset(0, 0, frame.width, frame.height);

		painter->begin();
		painter->push_state();
		painter->clear();

		window->on_draw(event);
		if (!event->is_blocked())
			Reflex::View_draw_tree(window->root(), *event, 0, frame.move_to(0));

		painter->pop_state();
		painter->end();
	}

	namespace global
	{

		static Window_CreateRootViewFun create_root_view_fun = NULL;

	}// global

	void
	Window_set_create_root_view_fun (Window_CreateRootViewFun fun)
	{
		global::create_root_view_fun = fun;
	}

	static View*
	create_root_view ()
	{
		return global::create_root_view_fun
			?	global::create_root_view_fun()
			:	new View();
	}

	void
	View_register_capture (View* view)
	{
		if (!view)
			argument_error(__FILE__, __LINE__);

		const Window* window = view->window();
		if (!window)
			invalid_state_error(__FILE__, __LINE__, "this view does not belong to any Window.");

		window->self->capturing_views[view] = true;
	}

	void
	View_unregister_capture (View* view)
	{
		if (!view)
			argument_error(__FILE__, __LINE__);

		const Window* window = view->window();
		if (!window) return;

		auto it = window->self->capturing_views.find(view);
		if (it == window->self->capturing_views.end()) return;

		it->second = false;
	}

	static void
	cleanup_capturing_views (Window* window)
	{
		auto end = window->self->capturing_views.end();
		for (auto it = window->self->capturing_views.begin(); it != end;)
		{
			auto t = it++;
			if (!t->second) window->self->capturing_views.erase(t);
		}
	}


	Window::Window ()
	:	self(Window_create_data())
	{
		Window_initialize(this);

		self->root.reset(create_root_view());
		self->root->set_name(VIEW_TAG_ROOT);
		View_set_window(self->root.get(), this);

		self->painter.canvas(0, 0, 1, 1, Window_get_pixel_density(*this));
	}

	Window::~Window ()
	{
		//close();

		View_set_window(self->root.get(), NULL);
	}

	void
	Window::show ()
	{
		int new_count = self->hide_count - 1;
		if (new_count == 0)
		{
			Event e;
			on_show(&e);
			if (e.is_blocked()) return;

			Window_show(this);
		}

		self->hide_count = new_count;
	}

	void
	Window::hide ()
	{
		int new_count = self->hide_count + 1;
		if (new_count == 1)
		{
			Event e;
			on_hide(&e);
			if (e.is_blocked()) return;

			Window_hide(this);
		}

		self->hide_count = new_count;
	}

	void
	Window::close ()
	{
		Event e;
		on_close(&e);
		if (e.is_blocked()) return;

		Window_close(this);
	}

	void
	Window::redraw ()
	{
		self->redraw = true;
	}

	void
	Window::set_title (const char* title)
	{
		Window_set_title(this, title);
	}

	const char*
	Window::title () const
	{
		return Window_get_title(*this);
	}

	void
	Window::set_frame (coord x, coord y, coord width, coord height)
	{
		Window_set_frame(this, x, y, width, height);
	}

	void
	Window::set_frame (const Bounds& bounds)
	{
		set_frame(bounds.x, bounds.y, bounds.width, bounds.height);
	}

	Bounds
	Window::frame () const
	{
		return Window_get_frame(*this);
	}

	void
	Window::set_resizable (bool state)
	{
		Window_set_resizable(this, state);
	}

	bool
	Window::is_resizable () const
	{
		return Window_is_resizable(*this);
	}

	bool
	Window::hidden () const
	{
		return self->hide_count > 0;
	}

	View*
	Window::root ()
	{
		return self->root.get();
	}

	const View*
	Window::root () const
	{
		return const_cast<Window*>(this)->root();
	}

	View*
	Window::focus ()
	{
		return self->focus.get();
	}

	const View*
	Window::focus () const
	{
		return const_cast<Window*>(this)->focus();
	}

	Painter*
	Window::painter ()
	{
		return &self->painter;
	}

	const Painter*
	Window::painter () const
	{
		return const_cast<Window*>(this)->painter();
	}

	Point
	Window::from_screen (const Point& point) const
	{
		not_implemented_error(__FILE__, __LINE__);
		return 0;
	}

	Point
	Window::to_screen (const Point& point) const
	{
		not_implemented_error(__FILE__, __LINE__);
		return 0;
	}

	void
	Window::on_show (Event* e)
	{
	}

	void
	Window::on_hide (Event* e)
	{
	}

	void
	Window::on_close (Event* e)
	{
	}

	void
	Window::on_update (UpdateEvent* e)
	{
	}

	void
	Window::on_draw (DrawEvent* e)
	{
	}

	void
	Window::on_move (FrameEvent* e)
	{
	}

	void
	Window::on_resize (FrameEvent* e)
	{
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
			case KeyEvent::NONE:                 break;
		}

		auto end = self->capturing_views.end();
		for (auto it = self->capturing_views.begin(); it != end; ++it)
		{
			KeyEvent event = *e;
			event.capture = true;
			View_call_key_event(const_cast<View*>(it->first.get()), event);
		}

		if (self->focus)
			View_call_key_event(self->focus.get(), *e);

		cleanup_capturing_views(this);
	}

	void
	Window::on_key_down (KeyEvent* e)
	{
	}

	void
	Window::on_key_up (KeyEvent* e)
	{
	}

	void
	Window::on_pointer (PointerEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);

		switch (e->action())
		{
			case PointerEvent::DOWN:   on_pointer_down(e);   break;
			case PointerEvent::UP:     on_pointer_up(e);     break;
			case PointerEvent::MOVE:   on_pointer_move(e);   break;
			case PointerEvent::CANCEL: on_pointer_cancel(e); break;
			default: break;
		}

		auto end = self->capturing_views.end();
		for (auto it = self->capturing_views.begin(); it != end; ++it)
		{
			PointerEvent event = *e;
			event.self->capture = true;
			for (auto& pointer : event.self->pointers)
				pointer.self->position = it->first.get()->from_window(pointer.self->position);

			View_call_pointer_event(const_cast<View*>(it->first.get()), event);
		}

		View_call_pointer_event(root(), *e);

		cleanup_capturing_views(this);
	}

	void
	Window::on_pointer_down (PointerEvent* e)
	{
	}

	void
	Window::on_pointer_up (PointerEvent* e)
	{
	}

	void
	Window::on_pointer_move (PointerEvent* e)
	{
	}

	void
	Window::on_pointer_cancel (PointerEvent* e)
	{
	}

	void
	Window::on_wheel (WheelEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);

		View_call_wheel_event(root(), *e);
	}

	Window::operator bool () const
	{
		return self && *self;
	}

	bool
	Window::operator ! () const
	{
		return !operator bool();
	}


}// Reflex
