#include "reflex/view.h"


#include <assert.h>
#include <algorithm>
#include <boost/scoped_ptr.hpp>
#include <xot/util.h>
#include "reflex/window.h"
#include "reflex/body.h"
#include "reflex/exception.h"
#include "world.h"


namespace Reflex
{


	bool set_style_owner (Style* style, View* owner);


	struct View::Data
	{

		typedef std::set<Selector> SelectorSet;

		enum Flags
		{

			ACTIVE               = 0x1 << 0,

			UPDATE_STYLE         = 0x1 << 1,

			APPLY_STYLE          = 0x1 << 2,

			UPDATE_LAYOUT        = 0x1 << 3,

			UPDATING_WORLD       = 0x1 << 4,

			REMOVE_SELF          = 0x1 << 5,

			HAS_VARIABLE_LENGTHS = 0x1 << 6,

			DEFAULT_FLAGS        = UPDATE_STYLE | UPDATE_LAYOUT

		};// Flags

		Window* window;

		View* parent;

		Bounds frame;

		float zoom, angle;

		ushort capture;

		short hide_count;

		uint flags;

		boost::scoped_ptr<Selector>    pselector;

		boost::scoped_ptr<SelectorSet> pselectors_for_update;

		boost::scoped_ptr<Point>       pscroll;

		boost::scoped_ptr<ChildList>   pchildren;

		boost::scoped_ptr<Style>       pstyle;

		boost::scoped_ptr<StyleList>   pstyles;

		boost::scoped_ptr<World>       pworld;

		boost::scoped_ptr<Body>        pbody;

		Data ()
		:	window(NULL), parent(NULL), zoom(1), angle(0),
			capture(CAPTURE_NONE), hide_count(0), flags(DEFAULT_FLAGS)
		{
		}

		~Data ()
		{
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

		Point& scroll ()
		{
			if (!pscroll) pscroll.reset(new Point);
			return *pscroll;
		}

		ChildList& children ()
		{
			if (!pchildren) pchildren.reset(new ChildList);
			return *pchildren;
		}

		Style& style (View* this_)
		{
			if (!pstyle)
			{
				pstyle.reset(new Style);
				set_style_owner(pstyle.get(), this_);
			}
			return *pstyle;
		}

		StyleList& styles ()
		{
			if (!pstyles) pstyles.reset(new StyleList);
			return *pstyles;
		}

		World* world (View* this_)
		{
			assert(this_);

			if (!pworld)
			{
				pworld.reset(new World(this_));
				resize_world(this_);
			}
			return pworld.get();
		}

		World* parent_world ()
		{
			if (!parent) return NULL;
			return parent->self->world(parent);
		}

		Body* body (View* this_)
		{
			if (!pbody)
			{
				World* world = parent_world();
				if (!world) return NULL;
				pbody.reset(world->create_body(this_, frame.position()));
				this_->make_body();
			}
			return pbody.get();
		}

		void resize_world (View* this_)
		{
			assert(this_);

			World* w = pworld.get();
			if (!w) return;

			w->resize(frame.width, frame.height);
			this_->redraw();
		}

		void update_body (View* this_)
		{
			assert(this_);

			Body* b = pbody.get();
			if (!b) return;

			b->set_transform(frame.x, frame.y, angle);
			this_->redraw();
		}

		void add_flag (uint flag)
		{
			flags |= flag;
		}

		void remove_flag (uint flag)
		{
			flags &= ~flag;
		}

		bool has_flag (uint flag) const
		{
			return flags & flag;
		}

		bool check_flag (uint flag)
		{
			bool result = has_flag(flag);
			remove_flag(flag);
			return result;
		}

	};// View::Data


	void
	set_window (View* view, Window* window_)
	{
		assert(view);

		Window* current = view->self->window;
		if (current == window_) return;

		if (current)
		{
			view->set_capture(View::CAPTURE_NONE);

			Event e;
			view->on_detach(&e);
		}

		view->self->window = window_;

		View::child_iterator end = view->child_end();
		for (View::child_iterator it = view->child_begin(); it != end; ++it)
			set_window(it->get(), window_);

		if (view->self->window)
		{
			Event e;
			view->on_attach(&e);
			view->resize_to_fit();
		}
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
		set_window(view, parent ? parent->window() : NULL);
	}

	bool
	is_view_active (View* view)
	{
		assert(view);

		return view->self->has_flag(View::Data::ACTIVE);
	}

	static bool
	remove_self (View* view)
	{
		assert(view);
		View::Data* self = view->self.get();

		if (!self->has_flag(View::Data::REMOVE_SELF) || !self->parent)
			return false;

		self->parent->remove_child(view);
		return true;
	}

	static void
	apply_style_to_children_have_variable_lengths (View* parent)
	{
		View::child_iterator end = parent->child_end();
		for (View::child_iterator it = parent->child_begin(); it != end; ++it)
		{
			View* child = *it;
			if (child->self->has_flag(View::Data::HAS_VARIABLE_LENGTHS))
				child->self->add_flag(View::Data::APPLY_STYLE);
		}
	}

	static void
	update_layout (View* view, bool update_parent = false)
	{
		assert(view);
		View::Data* self = view->self.get();

		self->add_flag(View::Data::UPDATE_LAYOUT);

		if (update_parent && self->parent)
			update_layout(self->parent);
	}

	static void
	update_view_frame (View* view, const Bounds& frame, float angle, bool update_body)
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

		if (update_body && (move || rotate))
			self->update_body(view);

		if (event.is_resize())
		{
			self->resize_world(view);
			view->on_resize(&event);

			apply_style_to_children_have_variable_lengths(view);
			update_layout(view, true);
		}

		view->redraw();
	}

	static void
	update_view_world (View* view, float dt)
	{
		assert(view);
		View::Data* self = view->self.get();

		World* world = self->pworld.get();
		if (world)
		{
			self->add_flag(View::Data::UPDATING_WORLD);
			world->step(dt);
			self->remove_flag(View::Data::UPDATING_WORLD);
		}

		Body* body = self->pbody.get();
		if (body)
		{
			Bounds b = view->frame();
			b.move_to(body->position());
			update_view_frame(view, b, body->angle(), false);
		}
	}

	void
	update_styles_for_selector (View* view, const Selector& selector)
	{
		assert(view);
		View::Data* self = view->self.get();

		if (selector.is_empty())
			self->add_flag(View::Data::UPDATE_STYLE);
		else
			self->selectors_for_update().insert(selector);
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

		View::Data::SelectorSet::iterator end = sels->end();
		for (View::Data::SelectorSet::iterator it = sels->begin(); it != end; ++it)
		{
			if (view_sel && view_sel->contains(*it))
				self->add_flag(View::Data::UPDATE_STYLE);

			view->find_children(&children, *it, true);
			for (View::ChildList::iterator jt = children.begin(); jt != children.end(); ++jt)
				(*jt)->self->add_flag(View::Data::UPDATE_STYLE);
		}

		sels->clear();
	}

	static void
	find_all_styles (
		View::StyleList* result, const View* view, const Selector& selector, bool recursive)
	{
		View::const_style_iterator end = view->style_end();
		for (View::const_style_iterator it = view->style_begin(); it != end; ++it)
		{
			if (selector.contains(it->selector()))
				result->push_back(*it);
		}

		if (recursive)
		{
			View::const_child_iterator end = view->child_end();
			for (View::const_child_iterator it = view->child_begin(); it != end; ++it)
				find_all_styles(result, it->get(), selector, true);
		}
	}

	static void
	get_styles_for_selector (
		View::StyleList* styles, View* view, const Selector& selector)
	{
		assert(styles);

		View* parent = view->parent();
		if (parent)
			get_styles_for_selector(styles, parent, selector);

		find_all_styles(styles, view, selector, false);
	}

	static bool
	get_styles_for_view (View::StyleList* styles, View* view)
	{
		assert(styles && view);

		styles->clear();

		Selector* sel = view->self->pselector.get();
		if (!sel || sel->is_empty())
			return false;

		get_styles_for_selector(styles, view, *sel);
		return !styles->empty();
	}

	static void
	update_view_style (View* view)
	{
		assert(view);
		View::Data* self = view->self.get();

		if (!self->check_flag(View::Data::UPDATE_STYLE))
			return;

		Style* style = self->pstyle.get();
		if (style)
		{
			void clear_inherited_values (Style*);
			clear_inherited_values(style);
		}

		View::StyleList styles;
		if (get_styles_for_view(&styles, view))
		{
			if (!style)
				style = &self->style(view);

			for (View::StyleList::iterator it = styles.begin(); it != styles.end(); ++it)
			{
				void override_style (Style*, const Style&);
				override_style(style, *it);
			}
		}

		bool has_variable_lengths (const Style&);
		if (style && has_variable_lengths(*style))
			self->add_flag(View::Data::HAS_VARIABLE_LENGTHS);
		else
			self->remove_flag(View::Data::HAS_VARIABLE_LENGTHS);

		if (style)
			self->add_flag(View::Data::APPLY_STYLE);
	}

	static void
	apply_view_style (View* view)
	{
		assert(view);

		View::Data* self = view->self.get();
		if (!self->check_flag(View::Data::APPLY_STYLE))
			return;

		Style* style = self->pstyle.get();
		if (!self) return;

		void apply_style (View* view, const Style& style);
		apply_style(view, *style);
	}

	static void reflow_children (View* parent, const FrameEvent* event = NULL);

	static void
	update_view_layout (View* view)
	{
		assert(view);
		View::Data* self = view->self.get();

		if (!self->check_flag(View::Data::UPDATE_LAYOUT))
			return;

		reflow_children(view);
	}

	void
	update_view_tree (View* view, const UpdateEvent& event)
	{
		if (!view)
			argument_error(__FILE__, __LINE__);

		if (event.is_blocked() || remove_self(view))
			return;

		UpdateEvent e = event;
		update_view_world(view, e.dt);
		view->on_update(&e);

		update_views_for_selectors(view);
		update_view_style(view);
		apply_view_style(view);

		View::child_iterator end = view->child_end();
		for (View::child_iterator it = view->child_begin(); it != end; ++it)
			update_view_tree(it->get(), e);

		update_view_layout(view);
	}

	static void
	draw_world (View* view, Painter* painter)
	{
		assert(view);

		World* world = view->self->pworld.get();
		if (world) world->draw(painter);
	}

	void
	draw_view_tree (
		View* view, const DrawEvent& event, const Point& offset, const Bounds& clip)
	{
		if (!view)
			argument_error(__FILE__, __LINE__);

		if (event.is_blocked() || view->hidden())
			return;

		DrawEvent e = event;
		Painter*  p = e.painter;
		Body*     b = view->self->pbody.get();

		p->push_matrix();
		p->push_attr();

		Bounds frame = view->frame();
		Point  pos   = frame.position() - view->scroll();
		p->translate(pos);

		float angle = view->self->angle;
		if (angle != 0) p->rotate(angle);

		float zoom = view->self->zoom;
		if (zoom != 1 && zoom > 0) p->scale(zoom, zoom);

		pos += offset;
		Bounds clip2 = clip & frame.move_to(pos);
		if (b)
			p->no_clip();
		else
			p->set_clip(clip2);

		e.view   = view;
		e.bounds = frame.move_to(0, 0, frame.z);
		view->on_draw(&e);

		View::child_iterator end = view->child_end();
		for (View::child_iterator it = view->child_begin(); it != end; ++it)
			draw_view_tree(it->get(), e, pos, clip2);

		draw_world(view, p);

		p->pop_attr();
		p->pop_matrix();
	}
#if 0
	void
	get_all_margins (MarginList* margins, View* view)
	{
		if (!margins || !view)
			argument_error(__FILE__, __LINE__);
	}

	void
	get_content_size (
		Point* size, View* view, const Point& parent_size, bool min_width, bool min_height)
	{
		if (!size || !view)
			argument_error(__FILE__, __LINE__);

		const Style&        style  = view->style();
		const StyleLength4& pos    = style.position();
		const StyleLength2& size   = style.size();
		const StyleLength4& margin = style.margin();
		bool need_width = true, need_height = true;
		coord n;

		size->reset(0);

		if (pos.left && pos.right)
		{
			if (!pos.left.is_fixed())
				layout_error(__FILE__, __LINE__, "");

			size->x += parent_size.x;
			need_width = false;
		}
		else if (size.width.get_pixel(&n, parent_size.x))
		{
			size->x += n;
			need_width = false;
		}

		if (pos.top && pos.bottom)
			size->y += parent_size.y;
		else if (size.height.get_pixel(&n, parent_size.y))
			size->y += n;
		else
			need_height = true;

		if (need_width || need_height)
		{
			Point content_size = view->content_size();
			bool view_width = false, view_height = false;

			if (need_width && content_size.x >= 0)
			{
				size->x += content_size.x;
				need_width = false;
			}

			if (need_height && content_size.y >= 0)
			{
				size->y += content_size.y;
				need_height = false;
			}

			if (need_width || need_height)
			{
				View::child_iterator end = view->child_end();
				for (View::child_iterator it = view->child_begin(); it != end; ++it)
				{
					const View* child = it->get();
					Point child_size;
					get_content_size(&child_size, child, );
					const Bounds& b = it->frame();
				}

				for ()
				Point view_size;
				get_view_size(&view_size, );
			}

			coord m;
			if (margin.left  .get_pixel(&m, parent_size.x)) size->x += m;
			if (margin.top   .get_pixel(&m, parent_size.y)) size->y += m;
			if (margin.right .get_pixel(&m, parent_size.x)) size->x += m;
			if (margin.bottom.get_pixel(&m, parent_size.y)) size->y += m;
		}







		const StyleLength4& margin = style.margin();

		coord left, top, right, bottom;
		bool have_left   = pos.left  .get_pixel(&left,   parent_width);
		bool have_top    = pos.top   .get_pixel(&top,    parent_height);
		bool have_right  = pos.right .get_pixel(&right,  parent_width);
		bool have_bottom = pos.bottom.get_pixel(&bottom, parent_height);

		if (have_left && have_right)
		{
			s
		}

		coord width, height;
		bool have_width  = size.width .get_pixel(&width,  parent_width);
		bool have_height = size.height.get_pixel(&height, parent_height);
		const StyleLength& w = style.width();
		switch (w.unit())
		{
			case StyleLength::PIXEL:
			case StyleLength::PERCENT:
			case StyleLength::NONE:
			default:
		}
	}

	static void
	get_flow_factor (int* h, int* v, Style::Flow flow)
	{
		if (!h || !v)
			argument_error(__FILE__, __LINE__);

		switch (flow)
		{
			case Style::FLOW_DOWN:  *h =  0; *v = +1; break;
			case Style::FLOW_RIGHT: *h = +1; *v =  0; break;
			case Style::FLOW_UP:    *h =  0; *v = -1; break;
			case Style::FLOW_LEFT:  *h = -1; *v =  0; break;
			default:                argument_error(__FILE__, __LINE__);
		}
	}

	struct Length4
	{

		coord l, t, r, b;

		Length4 ()
		{
		}

		Length4 (const StyleLength4& soruce)
		{
			reset(source);
		}

		void reset (const StyleLength4& source)
		{
			source.left  .get_pixel(&l, 0);
			source.top   .get_pixel(&t, 0);
			source.right .get_pixel(&r, 0);
			source.bottom.get_pixel(&b, 0);
		}

	};// Length4

	struct ChildView
	{

		View* view;

		Point size;

		Length4 margin;

		ChildView (View* view) : view(view) {}

	};// ChildView

	typedef std::vector<ChildView> ChildViewList;

	static void reflow_view_tree (Point* size, Length4* margin, View* view);

	static void
	reflow_children (View* view, Point* size, const Style& style)
	{
		assert(view && size);

		size_t nchildren = view->self->count_children();
		if (nchildren == 0) return;

		ChildViewList children;
		children.reserve(nchildren);

		View::child_iterator end = view->child_end();
		for (View::child_iterator it = view->child_begin(); it != end; ++it)
		{
			ChildView c(it->get());
			reflow_view_tree(&c.size, &c.margin, c.view);
			children.push_back(c);
		}

		assert(children.size() == nchildren)
#if 0
		Flow flow_main, flow_sub;
		style.get_flow(&flow_main, &flow_sub);

		int main_h, main_v, sub_h, sub_v;
		get_flow_factor(&main_h, &main_v, flow_main);
		get_flow_factor(&sub_h,  &sub_v,  flow_sub);
#endif
		coord x = 0, y = 0;
		coord parent_w = size->x, parent_h = size->y;
		coord height_max = 0;
		int child_count = 0;
		bool multiline = sub != Style::FLOW_NONE;

		for (size_t i = 0; i < nchildren; ++i)
		{
			const ChildView& child = children[i];
			Bounds& child_frame = child.view->self->frame;

			child_frame.x = x;
			child_frame.y = y;
			x            += child.size.x;

			if (child.size.y > size_max)
				size_max = child.size.y;

			if (parent_w >= 0 && x > parent_w && view_count > 0)
			{
				x        = 0;
				y       += size_max;
				size_max = 0;
				if (parent_h < 0 && y > size->y) size->y = y;
			}

			++view_count;
		}
	}

	static void
	reflow_view_tree (Point* size, Length4* margin, View* view)
	{
		if (!size || !margin || !view)
			argument_error(__FILE__, __LINE__);

		const Style& style = view->style();

		*size = view->content_size();
		reflow_children(view, size, style);

		Length4 padding(style.padding());
		size->x += padding.l + padding.r;
		size->y += padding.t + padding.b;

		Bounds& frame = view->self->frame;
		margin->reset(style.margin());

		frame.width  = margin.l + size->x + margin.r;
		frame.height = margin.t + size->y + margin.b;
	}

	void
	get_fixed_content_size (Point* size, View* view)
	{
		if (!size || !view)
			argument_error(__FILE__, __LINE__);

		const Style&        style  = view->style();
		const StyleLength2& size   = style.size();
		bool need_width = true, need_height = true;
		coord n;

		size->reset(-1);

		if (size.width.is_fixed() && size.width.get_pixel(&n, parent_size.x))
		{
			size->x += n;
			need_width = false;
		}

		if (size.height.get_pixel(&n, parent_size.y))
		{
			size->y += n;
			need_height = false;
		}

		if (need_width || need_height)
		{
			Point content_size = view->content_size();
			bool view_width = false, view_height = false;

			if (need_width && content_size.x >= 0)
			{
				size->x += content_size.x;
				need_width = false;
			}

			if (need_height && content_size.y >= 0)
			{
				size->y += content_size.y;
				need_height = false;
			}

			if (need_width || need_height)
			{
				View::child_iterator end = view->child_end();
				for (View::child_iterator it = view->child_begin(); it != end; ++it)
				{
					const View* child = it->get();
					Point child_size;
					get_content_size(&child_size, child, );
					const Bounds& b = it->frame();
				}

				for ()
				Point view_size;
				get_view_size(&view_size, );
			}

			coord m;
			if (margin.left  .get_pixel(&m, parent_size.x)) size->x += m;
			if (margin.top   .get_pixel(&m, parent_size.y)) size->y += m;
			if (margin.right .get_pixel(&m, parent_size.x)) size->x += m;
			if (margin.bottom.get_pixel(&m, parent_size.y)) size->y += m;
		}
	}
#endif

	static bool
	get_style_flow (Style::Flow* main, Style::Flow* sub, const Style* style)
	{
		void get_default_flow (Style::Flow*, Style::Flow*);

		assert(main && sub);

		if (style)
			style->get_flow(main, sub);
		else
			get_default_flow(main, sub);

		return *main != Style::FLOW_NONE;
	}

	struct LayoutContext
	{

		View* parent;

		const Bounds& parent_frame;

		const Style* parent_style;

		Style::Flow flow_main, flow_sub;

		coord x, y, size_max;

		int flow_count;

		Bounds child_frame;

		LayoutContext (View* parent)
		:	parent(parent), parent_frame(parent->self->frame),
			x(0), y(0), size_max(0), flow_count(0)
		{
			parent_style = parent->style();
			if (!get_style_flow(&flow_main, &flow_sub, parent_style))
				return;

#if 0
			int main_h, main_v, sub_h, sub_v;
			get_flow_factor(&main_h, &main_v, flow_main);
			get_flow_factor(&sub_h,  &sub_v,  flow_sub);
#endif
		}

		void place_child (View* child)
		{
			assert(child);

			Style* style = child->self->pstyle.get();
			Bounds frame = child->self->frame;

			if (!place_position(&frame, child, style))
				place_in_flow(&frame, child);

			child->set_frame(frame);
		}

		bool place_position (Bounds* frame, View* child, Style* style)
		{
			assert(frame && child);

			if (!style)
				return false;

			const StyleLength& l = style->left();
			const StyleLength& t = style->top();
			const StyleLength& r = style->right();
			const StyleLength& b = style->bottom();
			if (!l && !t && !r && !b)
				return false;

			bool get_pixel_length (
				coord* pixel_length,
				const StyleLength& style_length, const coord* parent_size);

			if (l && r)
			{
				coord ll, rr;
				get_pixel_length(&ll, l, &parent_frame.width);
				get_pixel_length(&rr, r, &parent_frame.width);
				frame->set_left(ll);
				frame->set_right(parent_frame.width - rr);
			}
			else if (l && !r)
				get_pixel_length(&frame->x, l, &parent_frame.width);
			else if (!l && r)
			{
				coord rr;
				get_pixel_length(&rr, r, &parent_frame.width);
				frame->move_to(parent_frame.width - (rr + frame->width), frame->y);
			}

			if (t && b)
			{
				coord tt, bb;
				get_pixel_length(&tt, t, &parent_frame.height);
				get_pixel_length(&bb, b, &parent_frame.height);
				frame->set_top(tt);
				frame->set_bottom(parent_frame.height - bb);
			}
			else if (t && !b)
				get_pixel_length(&frame->y, t, &parent_frame.height);
			else if (!t && b)
			{
				coord bb;
				get_pixel_length(&bb, b, &parent_frame.height);
				frame->move_to(frame->x, parent_frame.height - (bb + frame->height));
			}

			return true;
		}

		bool place_in_flow (Bounds* frame, View* child)
		{
			assert(frame && child);

			if (is_line_end(frame->width))
				break_line();

			frame->x = x;
			frame->y = y;

			x += frame->width;

			if (size_max < frame->height)
				size_max = frame->height;

			//if (y + size_max > parent_frame.height)
			//	parent_frame.height = y + size_max;

			++flow_count;
			return true;
		}

		void break_line ()
		{
			x          = 0;
			y         += size_max;
			size_max   = 0;
			flow_count = 0;
		}

		bool is_line_end (coord child_width) const
		{
			return
				is_multiline() &&
				flow_count > 0 &&
				(x + child_width) > parent_frame.width;
		}

		bool is_multiline () const
		{
			return flow_sub != Style::FLOW_NONE;
		}

	};// LayoutContext

	static void
	reflow_children (View* parent, const FrameEvent* event)
	{
		assert(parent);

		View::ChildList* children = parent->self->pchildren.get();
		if (!children || children->empty()) return;

		LayoutContext c(parent);

		View::child_iterator end = parent->child_end();
		for (View::child_iterator it = parent->child_begin(); it != end; ++it)
			c.place_child(it->get());
	}

	template <typename FUN, typename EVENT>
	static void
	call_children (View* parent, FUN fun, const EVENT& e)
	{
		assert(parent);

		View::child_iterator end = parent->child_end();
		for (View::child_iterator it = parent->child_begin(); it != end; ++it)
			fun(it->get(), e);
	}

	void
	call_key_event (View* child, const KeyEvent& event)
	{
		if (!child)
			argument_error(__FILE__, __LINE__);

		bool capturing = child->capture() & View::CAPTURE_KEY;
		if (capturing != event.capture) return;

		KeyEvent e = event;
		child->on_key(&e);
	}

	void
	call_pointer_event (View* child, const PointerEvent& event)
	{
		if (!child)
			argument_error(__FILE__, __LINE__);

		bool capturing = child->capture() & View::CAPTURE_POINTER;
		if (capturing != event.capture) return;

		const Bounds& frame = child->frame();

		if (!capturing)
		{
			bool include = false;
			for (size_t i = 0; i < event.size; ++i)
			{
				if (frame.is_include(event[i]))
				{
					include = true;
					break;
				}
			}
			if (!include) return;
		}

		PointerEvent e = event;
		Point offset = frame.position();
		for (size_t i = 0; i < e.size; ++i)
			e[i] -= offset;

		child->on_pointer(&e);

		if (!event.capture)
			call_children(child, call_pointer_event, e);
	}

	void
	call_wheel_event (View* child, const WheelEvent& event)
	{
		if (!child)
			argument_error(__FILE__, __LINE__);

		const Bounds& frame = child->frame();

		if (!frame.is_include(event.x, event.y, event.z))
			return;

		WheelEvent e = event;
		e.position() -= frame.position();

		child->on_wheel(&e);

		call_children(child, call_wheel_event, e);
	}


	View::View (const char* name)
	{
		if (name) self->selector().set_name(name);
	}

	View::~View ()
	{
	}

	void
	View::show ()
	{
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

		child->self->add_flag(View::Data::ACTIVE);

		self->children().push_back(child);
		set_parent(child, this);

		update_layout(this);
	}

	void
	View::remove_child (View* child)
	{
		if (!child || child == this)
			argument_error(__FILE__, __LINE__);

		if (!self->pchildren) return;

		child_iterator it = std::find(child_begin(), child_end(), child);
		if (it == child_end()) return;

		child->self->remove_flag(View::Data::ACTIVE);

		if (self->has_flag(Data::UPDATING_WORLD))
		{
			child->self->add_flag(View::Data::REMOVE_SELF);
			return;
		}

		if (child->parent() != this)
			invalid_state_error(__FILE__, __LINE__);

		child->clear_body();

		set_parent(child, NULL);
		self->pchildren->erase(it);

		update_layout(this);
	}

	static void
	find_all_children (
		View::ChildList* result, const View* view, const Selector& selector, bool recursive)
	{
		View::const_child_iterator end = view->child_end();
		for (View::const_child_iterator it = view->child_begin(); it != end; ++it)
		{
			const View* child = it->get();
			if (!child)
				invalid_state_error(__FILE__, __LINE__);

			if (child->selector().contains(selector))
				result->push_back(*it);

			if (recursive) find_all_children(result, child, selector, true);
		}
	}

	void
	View::find_children (
		ChildList* result, const Selector& selector, bool recursive) const
	{
		result->clear();
		find_all_children(result, this, selector, recursive);
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
		assert(view && *view);

		if (!set_style_owner(&style, view))
			return NULL;

		View::StyleList* styles = &view->self->styles();
		styles->push_back(style);
		return &styles->back();
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

		style_iterator it = std::find(style_begin(), style_end(), style);
		if (it == style_end()) return;

		self->styles().erase(it);
	}

	Style*
	View::get_style (const Selector& selector, bool create)
	{
		if (selector.is_empty())
			return style(create);

		style_iterator end = style_end();
		for (style_iterator it = style_begin(); it != end; ++it)
		{
			if (selector == it->selector())
				return &*it;
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

	void
	View::find_styles (StyleList* result, const Selector& selector, bool recursive) const
	{
		result->clear();
		find_all_styles(result, this, selector, recursive);
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

	void
	View::resize_to_fit ()
	{
		Point size = content_size();
		if (size.x < 0 && size.y < 0 && size.z <= 0) return;

		const Style* st = style();

		Bounds b = frame();
		if ((!st || !st->width())  && size.x >= 0) b.width  = size.x;
		if ((!st || !st->height()) && size.y >= 0) b.height = size.y;
		if (                          size.z >= 0) b.depth  = size.z;
		set_frame(b);
	}

	Point
	View::content_size () const
	{
		return -1;
	}

	void
	View::make_body ()
	{
		Body* b = body();
		if (!b) return;

		b->clear_fixtures();

		const Point& size = frame().size();
		b->add_box(size.x, size.y);
	}

	void
	View::clear_body ()
	{
		Body* body = self->pbody.get();
		if (!body) return;

		World* world = self->parent_world();
		if (!world)
			invalid_state_error(__FILE__, __LINE__);

		world->destroy_body(body);
		self->pbody.reset();
	}

	void
	View::set_name (const char* name)
	{
		self->selector().set_name(name);
	}

	const char*
	View::name () const
	{
		const Selector* sel = self->pselector.get();
		return sel ? sel->name() : "";
	}

	void
	View::add_tag (const char* tag)
	{
		self->selector().add_tag(tag);
	}

	void
	View::remove_tag (const char* tag)
	{
		Selector* sel = self->pselector.get();
		if (!sel) return;

		sel->remove_tag(tag);
	}

	static Selector::TagSet empty_tags;

	View::tag_iterator
	View::tag_begin ()
	{
		Selector* sel = self->pselector.get();
		return sel ? sel->begin() : empty_tags.begin();
	}

	View::const_tag_iterator
	View::tag_begin () const
	{
		const Selector* sel = self->pselector.get();
		return sel ? sel->begin() : empty_tags.begin();
	}

	View::tag_iterator
	View::tag_end ()
	{
		Selector* sel = self->pselector.get();
		return sel ? sel->end() : empty_tags.end();
	}

	View::const_tag_iterator
	View::tag_end () const
	{
		const Selector* sel = self->pselector.get();
		return sel ? sel->end() : empty_tags.end();
	}

	void
	View::set_selector (const Selector& selector)
	{
		self->selector() = selector;
	}

	Selector&
	View::selector ()
	{
		return self->selector();
	}

	const Selector&
	View::selector () const
	{
		static const Selector EMPTY;

		const Selector* sel = self->pselector.get();
		return sel ? *sel : EMPTY;
	}

	void
	View::set_frame (coord x, coord y, coord width, coord height)
	{
		set_frame(Bounds(x, y, width, height));
	}

	void
	View::set_frame (const Bounds& frame)
	{
		update_view_frame(this, frame, self->angle, true);
	}

	const Bounds&
	View::frame () const
	{
		return self->frame;
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

	Body*
	View::body ()
	{
		return self->body(this);
	}

	const Body*
	View::body () const
	{
		return const_cast<View*>(this)->body();
	}

	float
	View::meter2pixel (float meter, bool create_world)
	{
		Body* body = self->pbody.get();
		if (body)
			return body->meter2pixel(meter);

		World* world   = self->pworld.get();
		if (world)
			return world->meter2pixel(meter);

		View*  parent_      = NULL;
		World* parent_world = NULL;
		if (
			(parent_      = parent()) &&
			(parent_world = parent_->self->pworld.get()))
		{
			return parent_world->meter2pixel(meter);
		}

		if (!create_world)
			invalid_state_error(__FILE__, __LINE__);

		World* new_world = self->world(this);
		if (!new_world)
			invalid_state_error(__FILE__, __LINE__);

		return new_world->meter2pixel(meter);
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
		World* w = self->world(this);
		if (!w)
			invalid_state_error(__FILE__, __LINE__);

		w->set_gravity(vector);
	}

	Point
	View::gravity () const
	{
		World* w = self->pworld.get();
		return w ? w->gravity() : 0;
	}

	Body*
	View::wall ()
	{
		return self->world(this)->wall();
	}

	const Body*
	View::wall () const
	{
		return const_cast<View*>(this)->wall();
	}

	void
	View::set_debug (bool state)
	{
		World* w = self->world(this);
		if (!w)
			invalid_state_error(__FILE__, __LINE__);

		w->set_debug(state);
	}

	bool
	View::debugging () const
	{
		World* w = self->pworld.get();
		return w ? w->debugging() : false;
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

	void
	View::on_attach (Event* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_detach (Event* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_show (Event* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_hide (Event* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_update (UpdateEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_draw (DrawEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_move (FrameEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_resize (FrameEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_rotate (FrameEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_scroll (ScrollEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_focus (FocusEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_blur (FocusEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_key (KeyEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);

		switch (e->type)
		{
			case KeyEvent::DOWN: on_key_down(e); break;
			case KeyEvent::UP:   on_key_up(e);   break;
			case KeyEvent::NONE: break;
		}
	}

	void
	View::on_key_down (KeyEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_key_up (KeyEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_pointer (PointerEvent* e)
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
	}

	void
	View::on_pointer_down (PointerEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_pointer_up (PointerEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_pointer_move (PointerEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_wheel (WheelEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);

		//scroll_by(e->dx, e->dy, e->dz);
	}

	void
	View::on_capture (CaptureEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_contact (ContactEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);

		switch (e->type)
		{
			case ContactEvent::BEGIN: on_contact_begin(e); break;
			case ContactEvent::END:   on_contact_end(e);   break;
			case ContactEvent::NONE: break;
		}
	}

	void
	View::on_contact_begin (ContactEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	View::on_contact_end (ContactEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
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


}// Reflex
