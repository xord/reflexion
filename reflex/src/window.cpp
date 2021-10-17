#include "window.h"


#include <set>
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
	Window_register_capture (Window* window, View* view, Pointer::ID target)
	{
		assert(window);

		if (!view)
			argument_error(__FILE__, __LINE__);

		if (view->window() != window)
			invalid_state_error(__FILE__, __LINE__);

		if (target < 0) return;

		auto& targets = window->self->captures[view];
		if (std::find(targets.begin(), targets.end(), target) != targets.end())
			return;

		targets.insert(
			target == CAPTURE_ALL ? targets.begin() : targets.end(),
			target);
	}

	void
	Window_unregister_capture (Window* window, View* view, Pointer::ID target)
	{
		assert(window);

		if (!view)
			argument_error(__FILE__, __LINE__);

		auto captures_it = window->self->captures.find(view);
		if (captures_it == window->self->captures.end()) return;

		auto& targets   = captures_it->second;
		auto targets_it = std::find(targets.begin(), targets.end(), target);
		if (targets_it == targets.end()) return;

		targets.erase(targets_it);
	}

	static void
	cleanup_captures (Window* window)
	{
		assert(window);

		auto& caps = window->self->captures;
		for (auto it = caps.begin(), end = caps.end(); it != end;)
		{
			auto t = it++;
			if (t->second.empty()) caps.erase(t);
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

	void
	Window_call_key_event (Window* window, KeyEvent* event)
	{
		assert(window);

		if (!event)
			argument_error(__FILE__, __LINE__);

		window->on_key(event);

		switch (event->type)
		{
			case KeyEvent::DOWN: window->on_key_down(event); break;
			case KeyEvent::UP:   window->on_key_up(event);   break;
			default: break;
		}

		for (auto& pair : window->self->captures)
		{
			auto& targets = pair.second;
			if (targets.empty() || targets[0] != CAPTURE_ALL) continue;

			KeyEvent e = *event;
			e.capture = true;
			View_call_key_event(const_cast<View*>(pair.first.get()), e);
		}

		if (window->self->focus)
			View_call_key_event(window->self->focus.get(), *event);

		cleanup_captures(window);
	}

	using ViewList              = std::vector<View::Ref>;

	using PointerMap            = std::map<Pointer::ID, Pointer>;

	using ExtractedPointerIDSet = std::set<Pointer::ID>;

	using CaptureTargetIDList   = Window::Data::CaptureTargetIDList;

	static void
	get_views_capturing_all_pointers (Window* window, ViewList* result)
	{
		assert(window && result);

		result->clear();
		for (const auto& [view, targets] : window->self->captures)
		{
			if (!targets.empty() && targets[0] == CAPTURE_ALL)
				result->emplace_back(view);
		}
	}

	static void
	capture_all_pointers (
		Window* window, const PointerEvent& event, ViewList& views_capturing_all)
	{
		assert(window);

		if (views_capturing_all.empty()) return;

		PointerEvent_each_pointer(&event, [&](const auto& pointer)
		{
			if (pointer.action() == Pointer::DOWN)
			{
				for (auto& view : views_capturing_all)
					Window_register_capture(window, view, pointer.id());
			}
		});
	}

	static void
	add_extracted_pointer (
		PointerEvent* event, ExtractedPointerIDSet* extracteds,
		const Pointer& pointer)
	{
		assert(event && extracteds);

		PointerEvent_add_pointer(event, pointer);
		extracteds->insert(pointer.id());
	}

	static void
	extract_targeted_pointer_event (
		PointerEvent* event, ExtractedPointerIDSet* extracteds,
		const CaptureTargetIDList& targets, const PointerMap& pointers)
	{
		assert(event && event->empty() && extracteds);

		for (auto pointer_id : targets)
		{
			auto it = pointers.find(pointer_id);
			if (it != pointers.end())
				add_extracted_pointer(event, extracteds, it->second);
		}
	}

	static void
	call_targeted_pointer_events (
		Window* window,
		ExtractedPointerIDSet* extracteds, const PointerMap& pointers)
	{
		assert(window && extracteds);

		for (auto& [view, targets] : window->self->captures)
		{
			if (targets.empty()) continue;

			PointerEvent e;
			extract_targeted_pointer_event(&e, extracteds, targets, pointers);
			if (e.empty()) continue;

			PointerEvent_update_positions_for_capturing_views(&e, view);
			View_call_pointer_event(const_cast<View*>(view.get()), e);
		}
	}

	static void
	extract_uncaptured_pointer_event (
		PointerEvent* event, ExtractedPointerIDSet* extracteds,
		const PointerMap& pointers)
	{
		assert(event && event->empty() && extracteds);

		for (const auto& [_, pointer] : pointers)
		{
			// dragging pointers is captured as a targeted
			if (pointer.is_drag()) continue;

			add_extracted_pointer(event, extracteds, pointer);
		}
	}

	static void
	call_uncaptured_pointer_events (
		ExtractedPointerIDSet* extracteds,
		const ViewList& views_capturing_all, const PointerMap& pointers)
	{
		PointerEvent event;
		extract_uncaptured_pointer_event(&event, extracteds, pointers);
		if (event.empty()) return;

		for (auto& view : views_capturing_all)
		{
			PointerEvent e = event;
			PointerEvent_update_positions_for_capturing_views(&e, view);
			View_call_pointer_event(const_cast<View*>(view.get()), e);
		}
	}

	static void
	erase_extracted_pointers (
		PointerMap* pointers, const ExtractedPointerIDSet& extracteds)
	{
		assert(pointers);

		for (auto id : extracteds)
		{
			auto it = pointers->find(id);
			if (it != pointers->end()) pointers->erase(it);
		}
	}

	static void
	erase_extracted_pointers (
		PointerEvent* event, const ExtractedPointerIDSet& extracteds)
	{
		assert(event);

		for (auto id : extracteds)
			PointerEvent_erase_pointer(event, id);
	}

	static void
	call_captured_pointer_events (Window* window, PointerEvent* event)
	{
		assert(window && event);

		ViewList views_capturing_all;
		get_views_capturing_all_pointers(window, &views_capturing_all);
		capture_all_pointers(window, *event, views_capturing_all);

		PointerMap pointers;
		PointerEvent_each_pointer(event, [&](const auto& pointer)
		{
			if (pointer.id() >= 0) pointers[pointer.id()] = pointer;
		});

		ExtractedPointerIDSet extracteds;
		call_targeted_pointer_events(window, &extracteds, pointers);
		erase_extracted_pointers(&pointers, extracteds);

		call_uncaptured_pointer_events(&extracteds, views_capturing_all, pointers);
		erase_extracted_pointers(event, extracteds);
	}

	void
	Window_call_pointer_event (Window* window, PointerEvent* event)
	{
		assert(window);

		if (!event)
			argument_error(__FILE__, __LINE__);

		window->on_pointer(event);

		switch ((*event)[0].action())
		{
			case Pointer::DOWN:   window->on_pointer_down(event);   break;
			case Pointer::UP:     window->on_pointer_up(event);     break;
			case Pointer::MOVE:   window->on_pointer_move(event);   break;
			case Pointer::CANCEL: window->on_pointer_cancel(event); break;
			default: break;
		}

		call_captured_pointer_events(window, event);

		if (!event->empty())
			View_call_pointer_event(window->root(), *event);

		cleanup_captures(window);
	}

	void
	Window_call_wheel_event (Window* window, WheelEvent* event)
	{
		assert(window);

		if (!event)
			argument_error(__FILE__, __LINE__);

		window->on_wheel(event);

		View_call_wheel_event(window->root(), *event);
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

	Point
	Window::from_screen (const Point& point) const
	{
		return point - frame().position();
	}

	Point
	Window::to_screen (const Point& point) const
	{
		return point + frame().position();
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
