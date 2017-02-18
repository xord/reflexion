#include "view.h"


#include <assert.h>
#include <memory>
#include <algorithm>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Collision/Shapes/b2ChainShape.h>
#include <xot/util.h>
#include "reflex/window.h"
#include "reflex/timer.h"
#include "reflex/exception.h"
#include "reflex/debug.h"
#include "selector.h"
#include "timer.h"
#include "style.h"
#include "shape.h"
#include "world.h"
#include "body.h"
#include "fixture.h"


namespace Reflex
{


	static const char* WALL_NAME = "__WALL__";


	class WallShape : public Shape
	{

		typedef Shape Super;

		public:

			virtual void on_draw (DrawEvent* e)
			{
			}

		protected:

			virtual Fixture* create_fixtures ()
			{
				View* view = owner();
				if (!view)
					invalid_state_error(__FILE__, __LINE__);

				Bounds f  = frame();
				float ppm = view->meter2pixel();

				coord offset = 1;// hide wall
				coord x1 = to_b2coord(         - offset, ppm);
				coord y1 = to_b2coord(         - offset, ppm);
				coord x2 = to_b2coord(f.width  + offset, ppm);
				coord y2 = to_b2coord(f.height + offset, ppm);

				std::vector<b2Vec2> vecs;
				vecs.reserve(4);
				vecs.push_back(b2Vec2(x1, y1));
				vecs.push_back(b2Vec2(x2, y1));
				vecs.push_back(b2Vec2(x2, y2));
				vecs.push_back(b2Vec2(x1, y2));

				b2ChainShape b2shape;
				b2shape.CreateLoop(&vecs[0], 4);

				return FixtureBuilder(this, &b2shape).fixtures();
			}

	};// WallShape


	struct View::Data
	{

		typedef std::set<Selector> SelectorSet;

		enum Flags
		{

			ACTIVE               = Xot::bit(0),

			APPLY_STYLE          = Xot::bit(1),

			UPDATE_STYLE         = Xot::bit(2),

			UPDATE_SHAPES        = Xot::bit(3),

			UPDATE_LAYOUT        = Xot::bit(4),

			UPDATING_WORLD       = Xot::bit(5),

			RESIZE_TO_FIT        = Xot::bit(6),

			REMOVE_SELF          = Xot::bit(7),

			HAS_VARIABLE_LENGTHS = Xot::bit(8),

			NO_SHAPE             = Xot::bit(9),

			DEFAULT_FLAGS        = UPDATE_LAYOUT | UPDATE_STYLE

		};// Flags

		Window* window;

		View* parent;

		Bounds frame;

		float zoom, angle;

		ushort capture;

		short hide_count;

		uint flags;

		std::unique_ptr<Point>       pscroll;

		SelectorPtr                  pselector;

		std::unique_ptr<SelectorSet> pselectors_for_update;

		std::unique_ptr<Timers>      ptimers;

		std::unique_ptr<Style>       pstyle;

		std::unique_ptr<StyleList>   pstyles;

		Shape::Ref                   pshape;

		std::unique_ptr<ShapeList>   pshapes;

		std::unique_ptr<Body>        pbody;

		std::unique_ptr<World>       pchild_world;

		std::unique_ptr<ChildList>   pchildren;

		Data ()
		:	window(NULL), parent(NULL), zoom(1), angle(0),
			capture(CAPTURE_NONE), hide_count(0), flags(DEFAULT_FLAGS)
		{
		}

		~Data ()
		{
		}

		Point& scroll ()
		{
			if (!pscroll) pscroll.reset(new Point);
			return *pscroll;
		}

		Selector& selector ()
		{
			if (!pselector) pselector.reset(new Selector);
			return *pselector;
		}

		SelectorSet& selectors_for_update ()
		{
			if (!pselectors_for_update) pselectors_for_update.reset(new SelectorSet);
			return *pselectors_for_update;
		}

		Timers& timers ()
		{
			if (!ptimers) ptimers.reset(new Timers);
			return *ptimers;
		}

		Style& style (View* view)
		{
			if (!pstyle)
			{
				pstyle.reset(new Style);
				Style_set_owner(pstyle.get(), view);
			}
			return *pstyle;
		}

		StyleList& styles ()
		{
			if (!pstyles) pstyles.reset(new StyleList);
			return *pstyles;
		}

		ShapeList& shapes ()
		{
			if (!pshapes) pshapes.reset(new ShapeList);
			return *pshapes;
		}

		void update_shapes (bool force = false)
		{
			if (pshape)
				Shape_update_fixtures(pshape.get(), force);

			if (pshapes)
			{
				for (auto& shape : *pshapes)
					Shape_update_fixtures(shape.get(), force);
			}
		}

		void resize_shapes (FrameEvent* e)
		{
			if (pshape)
				pshape->on_resize(e);

			if (pshapes)
			{
				for (auto& shape : *pshapes)
					shape->on_resize(e);
			}
		}

		Body* body ()
		{
			if (!pbody)
			{
				World* w = parent_world();
				Body* b  = w
					? w->create_body(frame.position(), angle)
					: Body_create_temporary();
				assert(b);

				pbody.reset(b);
				update_body_frame();
			}
			return pbody.get();
		}

		void update_body_frame ()
		{
			if (!pbody) return;

			pbody->set_transform(frame.x, frame.y, angle);
		}

		void update_body_and_shapes (bool force = false)
		{
			std::unique_ptr<Body> old_body;
			if (pbody)
			{
				old_body = std::move(pbody);
				Body_copy_attributes(old_body.get(), body());
			}

			update_shapes(force);
		}

		World* parent_world (bool create = true)
		{
			if (!parent) return NULL;
			return parent->self->child_world(parent, create);
		}

		World* child_world (View* view, bool create = true)
		{
			assert(view);

			if (!pchild_world && create)
			{
				pchild_world.reset(new World());
				create_wall(view);
			}

			return pchild_world.get();
		}

		void create_wall (View* view)
		{
			assert(view);

			clear_walls(view);

			View* wall = new View(WALL_NAME);
			wall->set_shape(new WallShape());
			wall->set_static();

			view->add_child(wall);
		}

		void clear_walls (View* view)
		{
			assert(view);

			for (auto& wall : view->find_children(WALL_NAME))
				view->remove_child(wall.get());
		}

		ChildList& children ()
		{
			if (!pchildren) pchildren.reset(new ChildList);
			return *pchildren;
		}

		void add_flag (uint flag)
		{
			Xot::add_flag(&flags, flag);
		}

		void remove_flag (uint flag)
		{
			Xot::remove_flag(&flags, flag);
		}

		bool has_flag (uint flag) const
		{
			return Xot::has_flag(flags, flag);
		}

		bool check_and_remove_flag (uint flag)
		{
			return Xot::check_and_remove_flag(&flags, flag);
		}

	};// View::Data


	void
	View_set_window (View* view, Window* window)
	{
		assert(view);

		Window* current = view->self->window;
		if (current == window) return;

		if (current)
		{
			Event e;
			view->on_detach(&e);
			view->set_capture(View::CAPTURE_NONE);
		}

		view->self->window = window;
		view->self->update_body_and_shapes(!window);

		View::ChildList* pchildren = view->self->pchildren.get();
		if (pchildren)
		{
			for (auto& pchild : *pchildren)
				View_set_window(pchild.get(), window);
		}

		if (view->self->window)
		{
			Event e;
			view->on_attach(&e);
			view->self->add_flag(View::Data::RESIZE_TO_FIT);
		}
	}

	static void
	resize_view (View* view, FrameEvent* event)
	{
		assert(view);

		view->self->resize_shapes(event);
		view->on_resize(event);
	}

	static void
	apply_style_to_children_have_variable_lengths (View* parent)
	{
		assert(parent);

		View::ChildList* pchildren = parent->self->pchildren.get();
		if (!pchildren) return;

		for (auto& pchild : *pchildren)
		{
			assert(pchild);

			if (pchild->self->has_flag(View::Data::HAS_VARIABLE_LENGTHS))
				pchild->self->add_flag(View::Data::APPLY_STYLE);
		}
	}

	static void
	update_view_layout (View* view, bool update_parent = false)
	{
		assert(view);
		View::Data* self = view->self.get();

		self->add_flag(View::Data::UPDATE_LAYOUT);

		if (update_parent && self->parent)
			update_view_layout(self->parent);
	}

	static void
	update_view_frame (
		View* view, const Bounds& frame, float angle, bool update_body)
	{
		assert(view);
		View::Data* self = view->self.get();

		if (frame == self->frame && angle == self->angle)
			return;

		FrameEvent event(frame, self->frame, angle, self->angle);
		self->frame = frame;
		self->angle = angle;

		bool move = event.is_move(), rotate = event.is_rotate();

		if (move)   view->on_move(&event);
		if (rotate) view->on_rotate(&event);

		if (update_body && (move || rotate) && self->pbody)
			self->update_body_frame();

		if (event.is_resize())
		{
			resize_view(view, &event);
			apply_style_to_children_have_variable_lengths(view);
			update_view_layout(view, true);
		}

		view->redraw();
	}

	void
	View_set_frame (View* view, const Bounds& frame)
	{
		assert(view);

		update_view_frame(view, frame, view->self->angle, true);
	}

	const Style&
	View_get_style (View* view)
	{
		assert(view);

		static const Style DEFAULT_STYLE;
		return view->self->pstyle ? *view->self->pstyle : DEFAULT_STYLE;
	}

	Body*
	View_get_body (View* view, bool create)
	{
		if (!view) return NULL;

		return create ? view->self->body() : view->self->pbody.get();
	}

	bool
	View_is_active (const View& view)
	{
		return view.self->has_flag(View::Data::ACTIVE);
	}

	static void
	find_all_children (
		View::ChildList* result, const View* view, const Selector& selector,
		bool recursive)
	{
		assert(result && view);

		View::ChildList* pchildren = view->self->pchildren.get();
		if (!pchildren) return;

		for (auto& pchild : *pchildren)
		{
			if (!pchild)
				invalid_state_error(__FILE__, __LINE__);

			if (pchild->selector().contains(selector))
				result->push_back(pchild);

			if (recursive)
				find_all_children(result, pchild.get(), selector, true);
		}
	}

	static void
	find_all_styles (
		View::StyleList* result, const View* view, const Selector& selector,
		bool recursive)
	{
		assert(result && view);

		View::StyleList* pstyles = view->self->pstyles.get();
		if (pstyles)
		{
			for (auto& style : *pstyles)
			{
				if (selector.contains(style.selector()))
					result->push_back(style);
			}
		}

		if (!recursive) return;

		View::ChildList* pchildren = view->self->pchildren.get();
		if (pchildren)
		{
			for (auto& pchild : *pchildren)
				find_all_styles(result, pchild.get(), selector, true);
		}
	}

	static bool
	remove_self (View* view)
	{
		assert(view);
		View::Data* self = view->self.get();

		if (!self->parent)
			return false;

		self->parent->remove_child(view);
		return true;
	}

	static void
	fire_timers (View* view, double now)
	{
		assert(view);

		Timers* timers = view->self->ptimers.get();
		if (timers)
			timers->fire(now);
	}

	static void
	update_view_body (View* view)
	{
		assert(view);

		Body* body = view->self->pbody.get();
		if (!body) return;

		Bounds frame = view->frame();
		frame.move_to(body->position());
		update_view_frame(view, frame, body->angle(), false);
	}

	static void
	update_child_world (View* view, float dt)
	{
		assert(view);
		View::Data* self = view->self.get();

		World* child_world = self->pchild_world.get();
		if (!child_world) return;

		self->add_flag(View::Data::UPDATING_WORLD);
		child_world->on_update(dt);
		self->remove_flag(View::Data::UPDATING_WORLD);

		View::ChildList* pchildren = self->pchildren.get();
		if (pchildren)
		{
			for (auto& pchild : *pchildren)
				update_view_body(pchild.get());
		}
	}

	static void
	update_views_for_selectors (View* view)
	{
		assert(view);
		View::Data* self = view->self.get();

		View::Data::SelectorSet* sels = self->pselectors_for_update.get();
		if (!sels)
			return;

		Selector* view_sel = self->pselector.get();
		View::ChildList children;

		for (auto& sel : *sels)
		{
			if (view_sel && view_sel->contains(sel))
				self->add_flag(View::Data::UPDATE_STYLE);

			children.clear();
			find_all_children(&children, view, sel, true);
			for (auto& pchild : children)
				pchild->self->add_flag(View::Data::UPDATE_STYLE);
		}

		sels->clear();
	}

	static void
	get_styles_for_selector (
		View::StyleList* result, View* view, const Selector& selector)
	{
		assert(result);

		View* parent = view->parent();
		if (parent)
			get_styles_for_selector(result, parent, selector);

		find_all_styles(result, view, selector, false);
	}

	static bool
	get_styles_for_view (View::StyleList* result, View* view)
	{
		assert(result && view);

		result->clear();

		Selector* sel = view->self->pselector.get();
		if (!sel || sel->is_empty())
			return false;

		get_styles_for_selector(result, view, *sel);
		return !result->empty();
	}

	static void
	update_view_style (View* view)
	{
		assert(view);
		View::Data* self = view->self.get();

		Style* pstyle = self->pstyle.get();
		if (pstyle)
			Style_clear_inherited_values(pstyle);

		View::StyleList styles;
		if (get_styles_for_view(&styles, view))
		{
			if (!pstyle)
				pstyle = &self->style(view);

			for (auto& st : styles)
				Style_override(pstyle, st);
		}

		const Style& style = View_get_style(view);
		if (Style_has_variable_lengths(style))
			self->add_flag(View::Data::HAS_VARIABLE_LENGTHS);
		else
			self->remove_flag(View::Data::HAS_VARIABLE_LENGTHS);

		self->add_flag(View::Data::APPLY_STYLE);
	}

	static void
	update_view_shapes (View* view)
	{
		assert(view);
		View::Data* self = view->self.get();

		if (self->pbody && !self->pshape)
			view->shape();

		if (self->check_and_remove_flag(View::Data::UPDATE_SHAPES))
			self->update_shapes();
	}

	void
	View_update_tree (View* view, const UpdateEvent& event)
	{
		assert(view);
		View::Data* self = view->self.get();

		if (self->check_and_remove_flag(View::Data::REMOVE_SELF))
		{
			if (remove_self(view))
				return;
		}

		fire_timers(view, event.now);

		View::ChildList* pchildren = self->pchildren.get();
		if (pchildren)
		{
			for (auto& pchild : *pchildren)
				View_update_tree(pchild.get(), event);
		}

		update_child_world(view, event.dt);

		UpdateEvent e = event;
		view->on_update(&e);

		update_views_for_selectors(view);

		if (self->check_and_remove_flag(View::Data::UPDATE_STYLE))
			update_view_style(view);

		if (self->check_and_remove_flag(View::Data::APPLY_STYLE))
			Style_apply_to(View_get_style(view), view);

		if (self->check_and_remove_flag(View::Data::RESIZE_TO_FIT))
			view->resize_to_fit();

		if (self->check_and_remove_flag(View::Data::UPDATE_LAYOUT))
			view->update_layout();

		update_view_shapes(view);
	}

	static void
	setup_painter (Painter* painter, const Color& fill, const Color& stroke)
	{
		assert(painter);

		painter->set_fill(fill);
		painter->set_stroke(stroke);
	}

	static void
	draw_default_shape (View* view, DrawEvent* e)
	{
		assert(view && e && e->painter);

		const Style& style       = View_get_style(view);
		const Color& back_fill   = style.background_fill();
		const Color& back_stroke = style.background_stroke();

		Shape* shape = view->shape(false);
		if (shape)
		{
			setup_painter(e->painter, back_fill, back_stroke);
			shape->on_draw(e);
		}
		else if (back_fill.alpha > 0 || back_stroke.alpha > 0)
		{
			setup_painter(e->painter, back_fill, back_stroke);
			e->painter->rect(e->bounds);
		}
	}

	static void
	draw_view (View* view, DrawEvent* e)
	{
		assert(view && e && e->painter);

		draw_default_shape(view, e);

		const Style& style = View_get_style(view);
		setup_painter(
			e->painter, style.foreground_fill(), style.foreground_stroke());

		View::ShapeList* pshapes = view->self->pshapes.get();
		if (pshapes && !pshapes->empty())
		{
			for (auto& pshape : *pshapes)
				pshape->on_draw(e);
		}

		view->on_draw(e);
	}

	void
	View_draw_tree (
		View* view, const DrawEvent& event, const Point& offset, const Bounds& clip)
	{
		assert(view);

		if (event.is_blocked() || view->hidden())
			return;

		View::Data* self = view->self.get();
		DrawEvent e      = event;
		Painter* p       = e.painter;

		p->push_matrix();

		Bounds frame = view->frame();
		Point pos    = frame.position();

		const Point* scroll = view->self->pscroll.get();
		if (scroll) pos -= *scroll;

		p->translate(pos);

		float angle = self->angle;
		if (angle != 0)
			p->rotate(angle);

		float zoom = self->zoom;
		if (zoom != 1 && zoom > 0)
			p->scale(zoom, zoom);

		p->push_attrs();

		pos += offset;
		Bounds clip2 = clip & frame.move_to(pos);
		if (self->pbody)
			p->no_clip();
		else
			p->set_clip(clip2);

		e.view   = view;
		e.bounds = frame.move_to(0, 0, frame.z);
		draw_view(view, &e);

		p->pop_attrs();

		View::ChildList* pchildren = view->self->pchildren.get();
		if (pchildren)
		{
			for (auto& pchild : *pchildren)
				View_draw_tree(pchild.get(), e, pos, clip2);
		}

		World* child_world = view->self->pchild_world.get();
		if (child_world)
		{
			p->push_attrs();
			child_world->on_draw(p);
			p->pop_attrs();
		}

		p->pop_matrix();
	}

	void
	View_update_styles (View* view, const Selector& selector)
	{
		assert(view);
		View::Data* self = view->self.get();

		if (selector.is_empty())
			self->add_flag(View::Data::UPDATE_STYLE);
		else
			self->selectors_for_update().insert(selector);
	}

	void
	View_update_shapes (View* view)
	{
		assert(view);

		view->self->add_flag(View::Data::UPDATE_SHAPES);
	}

	template <typename FUN, typename EVENT>
	static void
	call_children (View* parent, FUN fun, const EVENT& e)
	{
		assert(parent);

		View::ChildList* pchildren = parent->self->pchildren.get();
		if (pchildren)
		{
			for (auto& pchild : *pchildren)
				fun(pchild.get(), e);
		}
	}

	void
	View_call_key_event (View* view, const KeyEvent& event)
	{
		assert(view);

		bool capturing = view->capture() & View::CAPTURE_KEY;
		if (capturing != event.capture) return;

		KeyEvent e = event;
		view->on_key(&e);

		switch (e.type)
		{
			case KeyEvent::DOWN: view->on_key_down(&e); break;
			case KeyEvent::UP:   view->on_key_up(&e);   break;
			case KeyEvent::NONE: break;
		}
	}

	static void
	filter_pointer_event (
		PointerEvent* to, const PointerEvent& from, const Bounds& frame)
	{
		assert(to);

		const Point& offset = frame.position();

		to->size = 0;
		for (size_t i = 0; i < from.size; ++i) {
			const Point& pos = from.position(i);
			if (!frame.is_include(pos))
				continue;

			to->positions[i] = pos - offset;
			++to->size;
		}
	}

	void
	View_call_pointer_event (View* view, const PointerEvent& event)
	{
		assert(view);

		bool capturing = view->capture() & View::CAPTURE_POINTER;
		if (capturing != event.capture) return;

		const Bounds& frame = view->frame();

		PointerEvent e = event;
		filter_pointer_event(&e, event, frame);

		if (!capturing && e.size == 0)
			return;

		view->on_pointer(&e);

		switch (e.type)
		{
			case PointerEvent::DOWN: view->on_pointer_down(&e); break;
			case PointerEvent::UP:   view->on_pointer_up(&e);   break;
			case PointerEvent::MOVE: view->on_pointer_move(&e); break;
			case PointerEvent::NONE: break;
		}

		if (!event.capture)
			call_children(view, View_call_pointer_event, e);
	}

	void
	View_call_wheel_event (View* view, const WheelEvent& event)
	{
		assert(view);

		const Bounds& frame = view->frame();

		if (!frame.is_include(event.x, event.y, event.z))
			return;

		WheelEvent e = event;
		e.position() -= frame.position();

		view->on_wheel(&e);

		call_children(view, View_call_wheel_event, e);
	}

	void
	View_call_contact_event (View* view, const ContactEvent& event)
	{
		assert(view);

		ContactEvent e = event;
		view->on_contact(&e);

		switch (e.type)
		{
			case ContactEvent::BEGIN: view->on_contact_begin(&e); break;
			case ContactEvent::END:   view->on_contact_end(&e);   break;
			case ContactEvent::NONE: break;
		}
	}


	View::View (const char* name)
	{
		if (name) set_name(name);
	}

	View::~View ()
	{
		clear_children();// to delete child shapes before world.
	}

	void
	View::show ()
	{
		if (self->hide_count <= SHRT_MIN)
			invalid_state_error(__FILE__, __LINE__);

		int new_count = self->hide_count - 1;
		if (new_count == 0)
		{
			Event e;
			on_show(&e);
			if (e.is_blocked()) return;

			redraw();
		}

		self->hide_count = new_count;
	}

	void
	View::hide ()
	{
		if (self->hide_count >= SHRT_MAX)
			invalid_state_error(__FILE__, __LINE__);

		int new_count = self->hide_count + 1;
		if (new_count == 1)
		{
			Event e;
			on_hide(&e);
			if (e.is_blocked()) return;

			redraw();
		}

		self->hide_count = new_count;
	}

	bool
	View::hidden () const
	{
		return self->hide_count > 0;
	}

	void
	View::redraw ()
	{
		Window* w = window();
		if (!w) return;

		w->redraw();
	}

	void
	View::focus (bool state)
	{
		Window* w = window();
		if (!w) return;

		void set_focus (Window*, View*);
		if (state)
			set_focus(w, this);
		else if (w->focus() == this)
			set_focus(w, NULL);
	}

	void
	View::blur ()
	{
		focus(false);
	}

	bool
	View::has_focus () const
	{
		const Window* w = window();
		return w && w->focus() == this;
	}

	Timer*
	View::start_timer (float seconds, int count)
	{
		return self->timers().add(this, seconds, count);
	}

	Timer*
	View::start_interval (float seconds)
	{
		return start_timer(seconds, -1);
	}

	void
	View::update_layout ()
	{
		LayoutContext(this).place_children();
	}

	Point
	View::from_parent (const Point& point) const
	{
		not_implemented_error(__FILE__, __LINE__);
		return 0;
	}

	Point
	View::to_parent (const Point& point) const
	{
		not_implemented_error(__FILE__, __LINE__);
		return 0;
	}

	Point
	View::from_window (const Point& point) const
	{
		Point p = point;
		for (const View* v = parent(); v; v = v->parent())
			p -= v->frame().position();
		return p;
	}

	Point
	View::to_window (const Point& point) const
	{
		not_implemented_error(__FILE__, __LINE__);
		return 0;
	}

	Point
	View::from_screen (const Point& point) const
	{
		not_implemented_error(__FILE__, __LINE__);
		return 0;
	}

	Point
	View::to_screen (const Point& point) const
	{
		not_implemented_error(__FILE__, __LINE__);
		return 0;
	}

	static void
	set_parent (View* view, View* parent)
	{
		assert(view);
		View::Data* self = view->self.get();

		View* current = self->parent;
		if (current == parent) return;

		if (current && parent)
		{
			reflex_error(__FILE__, __LINE__,
				"view '%s' already belongs to another parent '%s'.",
				view->name(), self->parent->name());
		}

		self->parent = parent;
		View_set_window(view, parent ? parent->window() : NULL);
	}

	void
	View::add_child (View* child)
	{
		if (!child || child == this)
			argument_error(__FILE__, __LINE__);

		bool found  = std::find(child_begin(), child_end(), child) != child_end();
		bool belong = child->parent() == this;
		if (found && belong)
			return;
		else if (found != belong)
			invalid_state_error(__FILE__, __LINE__);

		child->self->add_flag(Data::ACTIVE);

		self->children().push_back(child);
		set_parent(child, this);

		update_view_layout(this);
	}

	void
	View::remove_child (View* child)
	{
		if (!child || child == this)
			argument_error(__FILE__, __LINE__);

		if (!self->pchildren) return;

		auto end = child_end();
		auto it = std::find(child_begin(), end, child);
		if (it == end) return;

		child->self->remove_flag(Data::ACTIVE);

		if (self->has_flag(Data::UPDATING_WORLD))
		{
			child->self->add_flag(Data::REMOVE_SELF);
			return;
		}

		if (child->parent() != this)
			invalid_state_error(__FILE__, __LINE__);

		set_parent(child, NULL);

		self->pchildren->erase(it);
		if (self->pchildren->empty())
			self->pchildren.reset();

		update_view_layout(this);
	}

	void
	View::clear_children ()
	{
		while (self->pchildren && !self->pchildren->empty())
			remove_child(self->pchildren->begin()->get());
	}

	View::ChildList
	View::find_children (const Selector& selector, bool recursive) const
	{
		ChildList result;
		find_all_children(&result, this, selector, recursive);
		return result;
	}

	static View::ChildList empty_children;

	View::child_iterator
	View::child_begin ()
	{
		if (!self->pchildren) return empty_children.begin();
		return self->pchildren->begin();
	}

	View::const_child_iterator
	View::child_begin () const
	{
		if (!self->pchildren) return empty_children.begin();
		return self->pchildren->begin();
	}

	View::child_iterator
	View::child_end ()
	{
		if (!self->pchildren) return empty_children.end();
		return self->pchildren->end();
	}

	View::const_child_iterator
	View::child_end () const
	{
		if (!self->pchildren) return empty_children.end();
		return self->pchildren->end();
	}

	Style*
	View::style (bool create)
	{
		return create ? &self->style(this) : self->pstyle.get();
	}

	const Style*
	View::style () const
	{
		return const_cast<View*>(this)->style(false);
	}

	static Style*
	add_view_style (View* view, Style style)
	{
		assert(view);

		if (!Style_set_owner(&style, view))
			return NULL;

		View::StyleList* pstyles = &view->self->styles();
		pstyles->push_back(style);
		return &pstyles->back();
	}

	void
	View::add_style (const Style& style)
	{
		add_view_style(this, style);
	}

	void
	View::remove_style (const Style& style)
	{
		if (!self->pstyles) return;

		auto end = style_end();
		auto it = std::find(style_begin(), end, style);
		if (it == end) return;

		if (!Style_set_owner(&*it, NULL))
			invalid_state_error(__FILE__, __LINE__);

		self->pstyles->erase(it);
		if (self->pstyles->empty())
			self->pstyles.reset();
	}

	void
	View::clear_styles ()
	{
		while (self->pstyles && !self->pstyles->empty())
			remove_style(*self->pstyles->begin());
	}

	Style*
	View::get_style (const Selector& selector, bool create)
	{
		if (selector.is_empty())
			return style(create);

		StyleList* pstyles = self->pstyles.get();
		if (pstyles)
		{
			for (auto& style : *pstyles)
			{
				if (selector == style.selector())
					return &style;
			}
		}

		if (create)
		{
			Style s;
			s.set_selector(selector);
			return add_view_style(this, s);
		}

		return NULL;
	}

	const Style*
	View::get_style (const Selector& selector) const
	{
		return const_cast<View*>(this)->get_style(selector);
	}

	View::StyleList
	View::find_styles (const Selector& selector, bool recursive) const
	{
		StyleList result;
		find_all_styles(&result, this, selector, recursive);
		return result;
	}

	static View::StyleList empty_styles;

	View::style_iterator
	View::style_begin ()
	{
		if (!self->pstyles) return empty_styles.begin();
		return self->pstyles->begin();
	}

	View::const_style_iterator
	View::style_begin () const
	{
		if (!self->pstyles) return empty_styles.begin();
		return self->pstyles->begin();
	}

	View::style_iterator
	View::style_end ()
	{
		if (!self->pstyles) return empty_styles.end();
		return self->pstyles->end();
	}

	View::const_style_iterator
	View::style_end () const
	{
		if (!self->pstyles) return empty_styles.end();
		return self->pstyles->end();
	}

	static void
	set_shape_owner (Shape* shape, View* owner)
	{
		if (!shape || !Shape_set_owner(shape, owner))
			return;

		Event e;
		if (owner)
			shape->on_attach(&e);
		else
			shape->on_detach(&e);
	}

	void
	View::set_shape (Shape* shape)
	{
		if (!shape)
			self->add_flag(Data::NO_SHAPE);
		else
			self->remove_flag(Data::NO_SHAPE);

		Shape::Ref& pshape = self->pshape;
		if (shape == pshape.get()) return;

		set_shape_owner(pshape.get(), NULL);
		pshape.reset(shape);
		set_shape_owner(pshape.get(), this);
	}

	Shape*
	View::shape (bool create)
	{
		if (create && !self->pshape && !self->has_flag(Data::NO_SHAPE))
			set_shape(new RectShape);

		return self->pshape.get();
	}

	const Shape*
	View::shape () const
	{
		return const_cast<View*>(this)->shape(false);
	}

	void
	View::add_shape (Shape* shape)
	{
		if (!shape) return;

		set_shape_owner(shape, this);
		self->shapes().push_back(shape);
	}

	void
	View::remove_shape (Shape* shape)
	{
		if (!shape || !self->pshapes)
			return;

		auto end = shape_end();
		auto it  = std::find(shape_begin(), end, shape);
		if (it == end) return;

		set_shape_owner(it->get(), NULL);

		self->pshapes->erase(it);
		if (self->pshapes->empty())
			self->pshapes.reset();
	}

	void
	View::clear_shapes ()
	{
		while (self->pshapes && !self->pshapes->empty())
			remove_shape(self->pshapes->begin()->get());
	}

	View::ShapeList
	View::find_shapes (const Selector& selector) const
	{
		ShapeList result;
		ShapeList* pshapes = self->pshapes.get();
		if (pshapes)
		{
			for (auto& shape : *pshapes)
			{
				if (selector.contains(shape->selector()))
					result.push_back(shape);
			}
		}
		return result;
	}

	static View::ShapeList empty_shapes;

	View::shape_iterator
	View::shape_begin ()
	{
		if (!self->pshapes) return empty_shapes.begin();
		return self->pshapes->begin();
	}

	View::const_shape_iterator
	View::shape_begin () const
	{
		if (!self->pshapes) return empty_shapes.begin();
		return self->pshapes->begin();
	}

	View::shape_iterator
	View::shape_end ()
	{
		if (!self->pshapes) return empty_shapes.end();
		return self->pshapes->end();
	}

	View::const_shape_iterator
	View::shape_end () const
	{
		if (!self->pshapes) return empty_shapes.end();
		return self->pshapes->end();
	}

	void
	View::set_name (const char* name)
	{
		const char* current = this->name();
		if (name && current && strcmp(name, current) == 0)
			return;

		HasSelector::set_name(name);
		self->add_flag(Data::UPDATE_STYLE);
	}

	void
	View::add_tag (const char* tag)
	{
		if (has_tag(tag)) return;

		HasSelector::add_tag(tag);
		self->add_flag(Data::UPDATE_STYLE);
	}

	void
	View::remove_tag (const char* tag)
	{
		if (!has_tag(tag)) return;

		HasSelector::remove_tag(tag);
		self->add_flag(Data::UPDATE_STYLE);
	}

	void
	View::clear_tags ()
	{
		if (tag_begin() == tag_end()) return;

		HasSelector::clear_tags();
		self->add_flag(Data::UPDATE_STYLE);
	}

	void
	View::set_selector (const Selector& selector)
	{
		if (selector == this->selector()) return;

		HasSelector::set_selector(selector);
		self->add_flag(Data::UPDATE_STYLE);
	}

	void
	View::set_frame (coord x, coord y, coord width, coord height)
	{
		set_frame(Bounds(x, y, width, height));
	}

	void
	View::set_frame (const Bounds& frame)
	{
		static const StyleLength NONE(0, StyleLength::NONE);

		const Bounds& current = self->frame;

		if (frame.w != current.w && !Style_has_width(*style()))
			style()->set_width(NONE);

		if (frame.h != current.h && !Style_has_height(*style()))
			style()->set_height(NONE);

		View_set_frame(this, frame);
	}

	const Bounds&
	View::frame () const
	{
		return self->frame;
	}

	Point
	View::content_size () const
	{
		return -1;
	}

	void
	View::resize_to_fit ()
	{
		Point size = content_size();
		if (size.x < 0 && size.y < 0 && size.z <= 0) return;

		const Style* st = style(false);

		Bounds b = frame();
		if ((!st || !Style_has_width(*st))  && size.x >= 0) b.width  = size.x;
		if ((!st || !Style_has_height(*st)) && size.y >= 0) b.height = size.y;
		if (                                   size.z >= 0) b.depth  = size.z;
		set_frame(b);
	}

#if 0
	void
	View::set_angle (float degree)
	{
		update_view_frame(this, self->frame, degree, true);
	}
#endif

	float
	View::angle () const
	{
		return self->angle;
	}

	static const Point ZERO_SCROLL;

	void
	View::scroll_to (coord x, coord y, coord z)
	{
		Point old = self->scroll();
		self->scroll().reset(x, y, z);
		ScrollEvent e(x, y, z, x - old.x, y - old.y, z - old.z);
		on_scroll(&e);

		redraw();
	}

	void
	View::scroll_to (const Point& scroll)
	{
		scroll_to(scroll.x, scroll.y, scroll.z);
	}

	void
	View::scroll_by (coord x, coord y, coord z)
	{
		const Point& p = scroll();
		scroll_to(p.x + x, p.y + y, p.z + z);
	}

	void
	View::scroll_by (const Point& dscroll)
	{
		scroll_by(dscroll.x, dscroll.y, dscroll.z);
	}

	const Point&
	View::scroll () const
	{
		if (self->pscroll)
			return self->scroll();
		else
			return ZERO_SCROLL;
	}

	void
	View::set_zoom (float zoom)
	{
		self->zoom = zoom;
		redraw();
	}

	float
	View::zoom () const
	{
		return self->zoom;
	}

	void
	View::set_capture (uint types)
	{
		if (types == self->capture) return;

		uint old      = self->capture;
		self->capture = types;

		bool registered = old   != CAPTURE_NONE;
		bool capture    = types != CAPTURE_NONE;

		if (capture && !registered)
		{
			void register_capture (View*);
			register_capture(this);
		}
		else if (!capture && registered)
		{
			void unregister_capture (View*);
			unregister_capture(this);
		}

		CaptureEvent e(~old & types, old & ~types);
		on_capture(&e);
	}

	uint
	View::capture () const
	{
		return self->capture;
	}

	View*
	View::parent ()
	{
		return self->parent;
	}

	const View*
	View::parent () const
	{
		return const_cast<This*>(this)->parent();
	}

	Window*
	View::window ()
	{
		return self->window;
	}

	const Window*
	View::window () const
	{
		return const_cast<View*>(this)->window();
	}

	void
	View::apply_force (coord x, coord y)
	{
		self->body()->apply_force(x, y);
	}

	void
	View::apply_force (const Point& force)
	{
		self->body()->apply_force(force);
	}

	void
	View::apply_torque (float torque)
	{
		self->body()->apply_torque(torque);
	}

	void
	View::apply_linear_impulse (coord x, coord y)
	{
		self->body()->apply_linear_impulse(x, y);
	}

	void
	View::apply_linear_impulse (const Point& impulse)
	{
		self->body()->apply_linear_impulse(impulse);
	}

	void
	View::apply_angular_impulse (float impulse)
	{
		self->body()->apply_angular_impulse(impulse);
	}

	void
	View::set_static (bool state)
	{
		set_dynamic(!state);
	}

	bool
	View::is_static () const
	{
		return self->pbody ? !self->pbody->is_dynamic() : false;
	}

	void
	View::set_dynamic (bool state)
	{
		Body* b = self->body();
		if (!b || state == b->is_dynamic())
			return;

		b->set_dynamic(state);
	}

	bool
	View::is_dynamic () const
	{
		return self->pbody ? self->pbody->is_dynamic() : false;
	}

	void
	View::set_density (float density)
	{
		Shape* s = shape();
		if (!s)
			invalid_state_error(__FILE__, __LINE__, "view has no shape.");

		s->set_density(density);
	}

	float
	View::density () const
	{
		const Shape* s = self->pshape.get();
		return s ? s->density() : 0;
	}

	void
	View::set_friction (float friction)
	{
		Shape* s = shape();
		if (!s)
			invalid_state_error(__FILE__, __LINE__, "view has no shape.");

		s->set_friction(friction);
	}

	float
	View::friction () const
	{
		const Shape* s = self->pshape.get();
		return s ? s->friction() : 0;
	}

	void
	View::set_restitution (float restitution)
	{
		Shape* s = shape();
		if (!s)
			invalid_state_error(__FILE__, __LINE__, "view has no shape.");

		s->set_restitution(restitution);
	}

	float
	View::restitution () const
	{
		const Shape* s = self->pshape.get();
		return s ? s->restitution() : 0;
	}

	void
	View::set_sensor (bool state)
	{
		Shape* s = shape();
		if (!s)
			invalid_state_error(__FILE__, __LINE__, "view has no shape.");

		s->set_sensor(state);
	}

	bool
	View::is_sensor () const
	{
		const Shape* s = self->pshape.get();
		return s ? s->is_sensor() : false;
	}

	void
	View::set_category_bits (uint bits)
	{
		Shape* s = shape();
		if (!s)
			invalid_state_error(__FILE__, __LINE__, "view has no shape.");

		s->set_category_bits(bits);
	}

	uint
	View::category_bits () const
	{
		const Shape* s = self->pshape.get();
		return s ? s->category_bits() : 0x1;
	}

	void
	View::set_collision_mask (uint mask)
	{
		Shape* s = shape();
		if (!s)
			invalid_state_error(__FILE__, __LINE__, "view has no shape.");

		s->set_collision_mask(mask);
	}

	uint
	View::collision_mask () const
	{
		const Shape* s = self->pshape.get();
		return s ? s->collision_mask() : 0xffff;
	}

	void
	View::set_linear_velocity (coord x, coord y)
	{
		self->body()->set_linear_velocity(x, y);
	}

	void
	View::set_linear_velocity (const Point& velocity)
	{
		self->body()->set_linear_velocity(velocity);
	}

	Point
	View::linear_velocity () const
	{
		const Body* b = self->pbody.get();
		return b ? b->linear_velocity() : 0;
	}

	void
	View::set_angular_velocity (float velocity)
	{
		self->body()->set_angular_velocity(velocity);
	}

	float
	View::angular_velocity () const
	{
		const Body* b = self->pbody.get();
		return b ? b->angular_velocity() : 0;
	}

	void
	View::set_gravity_scale (float scale)
	{
		self->body()->set_gravity_scale(scale);
	}

	float
	View::gravity_scale () const
	{
		const Body* b = self->pbody.get();
		return b ? b->gravity_scale() : 1;
	}

	float
	View::meter2pixel (float meter, bool create_world)
	{
		Body* body = self->pbody.get();
		if (body)
			return body->meter2pixel(meter);

		World* child_world = self->pchild_world.get();
		if (child_world)
			return child_world->meter2pixel(meter);

		World* parent_world = self->parent_world(false);
		if (parent_world)
			return parent_world->meter2pixel(meter);

		if (!create_world)
			invalid_state_error(__FILE__, __LINE__);

		child_world = self->child_world(this);
		if (!child_world)
			invalid_state_error(__FILE__, __LINE__);

		return child_world->meter2pixel(meter);
	}

	float
	View::meter2pixel (float meter) const
	{
		return const_cast<View*>(this)->meter2pixel(meter, false);
	}

	void
	View::set_gravity (float x, float y)
	{
		set_gravity(Point(x, y));
	}

	void
	View::set_gravity (const Point& vector)
	{
		self->child_world(this)->set_gravity(vector);
	}

	Point
	View::gravity () const
	{
		World* w = self->pchild_world.get();
		return w ? w->gravity() : 0;
	}

	View*
	View::wall ()
	{
		self->child_world(this);

		ChildList children = find_children(WALL_NAME);
		if (children.empty()) return NULL;

		return children[0].get();
	}

	const View*
	View::wall () const
	{
		return const_cast<View*>(this)->wall();
	}

	void
	View::set_debug (bool state)
	{
		World* w = self->child_world(this);
		if (w) w->set_debug(state);
	}

	bool
	View::debugging () const
	{
		World* w = self->pchild_world.get();
		return w ? w->debugging() : false;
	}

	void
	View::on_attach (Event* e)
	{
	}

	void
	View::on_detach (Event* e)
	{
	}

	void
	View::on_show (Event* e)
	{
	}

	void
	View::on_hide (Event* e)
	{
	}

	void
	View::on_update (UpdateEvent* e)
	{
	}

	void
	View::on_draw (DrawEvent* e)
	{
	}

	void
	View::on_move (FrameEvent* e)
	{
	}

	void
	View::on_resize (FrameEvent* e)
	{
	}

	void
	View::on_rotate (FrameEvent* e)
	{
	}

	void
	View::on_scroll (ScrollEvent* e)
	{
	}

	void
	View::on_focus (FocusEvent* e)
	{
	}

	void
	View::on_blur (FocusEvent* e)
	{
	}

	void
	View::on_key (KeyEvent* e)
	{
	}

	void
	View::on_key_down (KeyEvent* e)
	{
	}

	void
	View::on_key_up (KeyEvent* e)
	{
	}

	void
	View::on_pointer (PointerEvent* e)
	{
	}

	void
	View::on_pointer_down (PointerEvent* e)
	{
	}

	void
	View::on_pointer_up (PointerEvent* e)
	{
	}

	void
	View::on_pointer_move (PointerEvent* e)
	{
	}

	void
	View::on_wheel (WheelEvent* e)
	{
		//scroll_by(e->dx, e->dy, e->dz);
	}

	void
	View::on_capture (CaptureEvent* e)
	{
	}

	void
	View::on_timer (TimerEvent* e)
	{
	}

	void
	View::on_contact (ContactEvent* e)
	{
	}

	void
	View::on_contact_begin (ContactEvent* e)
	{
	}

	void
	View::on_contact_end (ContactEvent* e)
	{
	}

	View::operator bool () const
	{
		return true;
	}

	bool
	View::operator ! () const
	{
		return !operator bool();
	}

	SelectorPtr*
	View::get_selector_ptr ()
	{
		return &self->pselector;
	}


}// Reflex
