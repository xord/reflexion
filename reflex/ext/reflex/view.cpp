#include "reflex/ruby/view.h"


#include <vector>
#include <rucy.h>
#include <rays/ruby/point.h>
#include <rays/ruby/bounds.h>
#include "reflex/ruby/window.h"
#include "reflex/ruby/selector.h"
#include "reflex/ruby/style.h"
#include "reflex/ruby/timer.h"
#include "reflex/ruby/body.h"


using namespace Rucy;

using Reflex::coord;


RUCY_DEFINE_WRAPPER_VALUE_FROM_TO(Reflex::View)

#define   THIS    to<      Reflex::View*>(self)
#define C_THIS    to<const Reflex::View*>(self)

#define CHECK     RUCY_CHECK_OBJECT(Reflex::View, self)

#define CALL(fun) RUCY_WRAPPER_CALL(Reflex::View, THIS, fun)


template <typename T>
static inline Value
array (T begin, T end)
{
	std::vector<Value> v;
	for (T it = begin; it != end; ++it) v.push_back(value(*it));
	return value(v.size(), &v[0]);
}


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return value(new Reflex::RubyView<Reflex::View>, klass);
}
RUCY_END

static
RUCY_DEF0(show)
{
	CHECK;
	THIS->show();
	return self;
}
RUCY_END

static
RUCY_DEF0(hide)
{
	CHECK;
	THIS->hide();
	return self;
}
RUCY_END

static
RUCY_DEF0(hidden)
{
	CHECK;
	return value(THIS->hidden());
}
RUCY_END

static
RUCY_DEF0(redraw)
{
	CHECK;
	THIS->redraw();
	return self;
}
RUCY_END

static
RUCY_DEFN(focus)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "View#focus", argc, 0, 1);

	bool state = (argc >= 1) ? to<bool>(argv[0]) : true;

	THIS->focus(state);
	return self;
}
RUCY_END

static
RUCY_DEF0(blur)
{
	CHECK;
	THIS->blur();
	return self;
}
RUCY_END

static
RUCY_DEF0(has_focus)
{
	CHECK;
	return value(THIS->has_focus());
}
RUCY_END

static
RUCY_DEFN(start_timer)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "View#start_timer", argc, 1, 2);

	Reflex::Timer* timer = THIS->start_timer(
		argv[0].as_f(true),
		argc >= 2 ? argv[1].as_i() : 1);

	return value(timer);
}
RUCY_END

static
RUCY_DEF1(add_child, child)
{
	CHECK;
	THIS->add_child(to<Reflex::View*>(child));
	return child;
}
RUCY_END

static
RUCY_DEF1(remove_child, child)
{
	CHECK;
	THIS->remove_child(to<Reflex::View*>(child));
	return child;
}
RUCY_END

static
RUCY_DEFN(find_children)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "View#find_children", argc, 1, 2);

	bool recursive = (argc >= 2) ? to<bool>(argv[1]) : false;

	Reflex::View::ChildList children;
	THIS->find_children(&children, to<Reflex::Selector>(argv[0]), recursive);
	return array(children.begin(), children.end());
}
RUCY_END

static
RUCY_DEF0(each_child)
{
	CHECK;

	Value ret;
	Reflex::View::child_iterator end = THIS->child_end();
	for (Reflex::View::child_iterator it = THIS->child_begin(); it != end; ++it)
		ret = rb_yield(value(it->get()));
	return ret;
}
RUCY_END

static
RUCY_DEF1(add_style, style)
{
	CHECK;
	THIS->add_style(to<Reflex::Style&>(style));
	return style;
}
RUCY_END

static
RUCY_DEF1(remove_style, style)
{
	CHECK;
	THIS->remove_style(to<Reflex::Style&>(style));
	return style;
}
RUCY_END

static
RUCY_DEF1(get_style, selector)
{
	CHECK;

	Reflex::Style* s = NULL;
	if (selector)
		s = THIS->get_style(to<Reflex::Selector>(selector), true);
	else
		s = THIS->style(true);

	return s ? value(*s) : nil();
}
RUCY_END

static
RUCY_DEFN(find_styles)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "View#find_styles", argc, 1, 2);

	bool recursive = (argc >= 2) ? to<bool>(argv[1]) : false;

	Reflex::View::StyleList styles;
	THIS->find_styles(&styles, to<Reflex::Selector>(argv[0]), recursive);
	return array(styles.begin(), styles.end());
}
RUCY_END

static
RUCY_DEF0(each_style)
{
	CHECK;

	Value ret;
	Reflex::View::style_iterator end = THIS->style_end();
	for (Reflex::View::style_iterator it = THIS->style_begin(); it != end; ++it)
		ret = rb_yield(value(*it));
	return ret;
}
RUCY_END

static
RUCY_DEF1(set_name, name)
{
	CHECK;
	THIS->set_name(name.is_nil() ? NULL : name.c_str());
}
RUCY_END

static
RUCY_DEF0(get_name)
{
	CHECK;
	return THIS->name() ? value(THIS->name()) : nil();
}
RUCY_END

static
RUCY_DEF1(add_tag, tag)
{
	CHECK;
	THIS->add_tag(tag.c_str());
}
RUCY_END

static
RUCY_DEF1(remove_tag, tag)
{
	CHECK;
	THIS->remove_tag(tag.c_str());
}
RUCY_END

static
RUCY_DEF1(has_tag, tag)
{
	CHECK;
	return value(THIS->has_tag(tag.c_str()));
}
RUCY_END

static
RUCY_DEF0(each_tag)
{
	CHECK;

	Value ret;
	Reflex::View::const_tag_iterator end = C_THIS->tag_end();
	for (Reflex::View::const_tag_iterator it = C_THIS->tag_begin(); it != end; ++it)
		ret = rb_yield(value(*it));
	return ret;
}
RUCY_END

static
RUCY_DEF1(set_selector, selector)
{
	CHECK;
	THIS->set_selector(to<Reflex::Selector>(selector));
}
RUCY_END

static
RUCY_DEF0(get_selector)
{
	CHECK;
	return value(C_THIS->selector());
}
RUCY_END

static
RUCY_DEFN(set_frame)
{
	CHECK;
	THIS->set_frame(to<Rays::Bounds>(argc, argv));
	return value(THIS->frame());
}
RUCY_END

static
RUCY_DEF0(get_frame)
{
	CHECK;
	return value(THIS->frame());
}
RUCY_END

static
RUCY_DEF0(content_size)
{
	CHECK;
	return value(CALL(content_size()));
}
RUCY_END

static
RUCY_DEF0(resize_to_fit)
{
	CHECK;
	THIS->resize_to_fit();
	return self;
}
RUCY_END

static
RUCY_DEF1(set_zoom, zoom)
{
	CHECK;
	THIS->set_zoom(to<float>(zoom));
}
RUCY_END

static
RUCY_DEF0(get_zoom)
{
	CHECK;
	return value(THIS->zoom());
}
RUCY_END

static
RUCY_DEF0(get_angle)
{
	CHECK;
	return value(THIS->angle());
}
RUCY_END

static
RUCY_DEF1(set_capture, types)
{
	CHECK;
	THIS->set_capture(to<uint>(types));
}
RUCY_END

static
RUCY_DEF0(get_capture)
{
	CHECK;
	return value(THIS->capture());
}
RUCY_END

static
RUCY_DEFN(scroll_to)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "View#scroll_to", argc, 1, 2, 3);

	if (argv[0].is_kind_of(Rays::point_class()))
		THIS->scroll_to(to<Rays::Point&>(argv[0]));
	else
	{
		const Rays::Point& p = THIS->scroll();
		coord x = (argc >= 1 && argv[0]) ? to<coord>(argv[0]) : p.x;
		coord y = (argc >= 2 && argv[1]) ? to<coord>(argv[1]) : p.y;
		coord z = (argc >= 3 && argv[2]) ? to<coord>(argv[2]) : p.z;
		THIS->scroll_to(x, y, z);
	}

	return self;
}
RUCY_END

static
RUCY_DEFN(scroll_by)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "View#scroll_by", argc, 1, 2, 3);

	if (argv[0].is_kind_of(Rays::point_class()))
		THIS->scroll_by(to<Rays::Point&>(argv[0]));
	else
	{
		coord x = (argc >= 1 && argv[0]) ? to<coord>(argv[0]) : 0;
		coord y = (argc >= 2 && argv[1]) ? to<coord>(argv[1]) : 0;
		coord z = (argc >= 3 && argv[2]) ? to<coord>(argv[2]) : 0;
		THIS->scroll_by(x, y, z);
	}

	return self;
}
RUCY_END

static
RUCY_DEF0(get_scroll)
{
	CHECK;
	return value(THIS->scroll());
}
RUCY_END

static
RUCY_DEF0(parent)
{
	CHECK;
	return value(THIS->parent());
}
RUCY_END

static
RUCY_DEF0(window)
{
	CHECK;
	return value(THIS->window());
}
RUCY_END

static
RUCY_DEF0(make_body)
{
	CHECK;
	CALL(make_body());
}
RUCY_END

static
RUCY_DEF0(clear_body)
{
	CHECK;
	THIS->clear_body();
}
RUCY_END

static
RUCY_DEF0(body)
{
	CHECK;
	return value(THIS->body());
}
RUCY_END

static
RUCY_DEFN(meter2pixel)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "View#meter2pixel", argc, 0, 1, 2);

	float meter       = argc >= 1 ? argv[0].as_f(true) : 1;
	bool create_world = argc >= 2 ? (bool) argv[1]     : true;
	return value(THIS->meter2pixel(meter, create_world));
}
RUCY_END

static
RUCY_DEFN(set_gravity)
{
	CHECK;
	THIS->set_gravity(to<Rays::Point>(argc, argv));
	return value(THIS->gravity());
}
RUCY_END

static
RUCY_DEF0(get_gravity)
{
	CHECK;
	return value(THIS->gravity());
}
RUCY_END

static
RUCY_DEF0(wall)
{
	CHECK;
	return value(THIS->wall());
}
RUCY_END

static
RUCY_DEF1(set_debug, state)
{
	CHECK;
	THIS->set_debug(state);
	return state;
}
RUCY_END

static
RUCY_DEF0(is_debugging)
{
	CHECK;
	return value(THIS->debugging());
}
RUCY_END

static
RUCY_DEF1(on_attach, event)
{
	CHECK;

	RUCY_SYM(call_attach_block);
	self.call(call_attach_block);

	CALL(on_attach(to<Reflex::Event*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_detach, event)
{
	CHECK;
	CALL(on_detach(to<Reflex::Event*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_show, event)
{
	CHECK;
	CALL(on_show(to<Reflex::Event*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_hide, event)
{
	CHECK;
	CALL(on_hide(to<Reflex::Event*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_update, event)
{
	CHECK;
	CALL(on_update(to<Reflex::UpdateEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_draw, event)
{
	CHECK;
	CALL(on_draw(to<Reflex::DrawEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_move, event)
{
	CHECK;
	CALL(on_move(to<Reflex::FrameEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_resize, event)
{
	CHECK;
	CALL(on_resize(to<Reflex::FrameEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_rotate, event)
{
	CHECK;
	CALL(on_rotate(to<Reflex::FrameEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_scroll, event)
{
	CHECK;
	CALL(on_scroll(to<Reflex::ScrollEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_focus, event)
{
	CHECK;
	CALL(on_focus(to<Reflex::FocusEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_blur, event)
{
	CHECK;
	CALL(on_blur(to<Reflex::FocusEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_key, event)
{
	CHECK;
	CALL(on_key(to<Reflex::KeyEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_key_down, event)
{
	CHECK;
	CALL(on_key_down(to<Reflex::KeyEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_key_up, event)
{
	CHECK;
	CALL(on_key_up(to<Reflex::KeyEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_pointer, event)
{
	CHECK;
	CALL(on_pointer(to<Reflex::PointerEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_pointer_down, event)
{
	CHECK;
	CALL(on_pointer_down(to<Reflex::PointerEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_pointer_up, event)
{
	CHECK;
	CALL(on_pointer_up(to<Reflex::PointerEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_pointer_move, event)
{
	CHECK;
	CALL(on_pointer_move(to<Reflex::PointerEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_wheel, event)
{
	CHECK;
	CALL(on_wheel(to<Reflex::WheelEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_capture, event)
{
	CHECK;
	CALL(on_capture(to<Reflex::CaptureEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_timer, event)
{
	CHECK;
	CALL(on_timer(to<Reflex::TimerEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_contact, event)
{
	CHECK;
	CALL(on_contact(to<Reflex::ContactEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_contact_begin, event)
{
	CHECK;
	CALL(on_contact_begin(to<Reflex::ContactEvent*>(event)));
}
RUCY_END

static
RUCY_DEF1(on_contact_end, event)
{
	CHECK;
	CALL(on_contact_end(to<Reflex::ContactEvent*>(event)));
}
RUCY_END

static
RUCY_DEF_clear_override_flags(cof, Reflex::View);


static Class cView;

void
Init_view ()
{
	Module mReflex = define_module("Reflex");

	cView = mReflex.define_class("View");
	cView.define_alloc_func(alloc);
	cView.define_method("show",    show);
	cView.define_method("hide",    hide);
	cView.define_method("hidden?", hidden);
	cView.define_method("redraw",  redraw);
	cView.define_method("focus",  focus);
	cView.define_method("blur",   blur);
	cView.define_method("focus?", has_focus);
	cView.define_private_method("start_timer", start_timer);
	cView.define_method("add_child",     add_child);
	cView.define_method("remove_child",  remove_child);
	cView.define_method("find_children", find_children);
	cView.define_method("each_child",    each_child);
	cView.define_method("add_style",    add_style);
	cView.define_method("remove_style", remove_style);
	cView.define_method("get_style",    get_style);
	cView.define_method("find_styles",  find_styles);
	cView.define_method("each_style",   each_style);
	cView.define_method("name=", set_name);
	cView.define_method("name",  get_name);
	cView.define_method("add_tag",    add_tag);
	cView.define_method("remove_tag", remove_tag);
	cView.define_method("tagged?",    has_tag);
	cView.define_method("each_tag", each_tag);
	cView.define_method("selector=", set_selector);
	cView.define_method("selector",  get_selector);
	cView.define_method("frame=", set_frame);
	cView.define_method("frame",  get_frame);
	cView.define_method("content_size",  content_size);
	cView.define_method("resize_to_fit", resize_to_fit);
	cView.define_method("zoom=", set_zoom);
	cView.define_method("zoom",  get_zoom);
	cView.define_method("angle", get_angle);
	cView.define_method("scroll_to", scroll_to);
	cView.define_method("scroll_by", scroll_by);
	cView.define_method("scroll", get_scroll);
	cView.define_method("capture=", set_capture);
	cView.define_method("capture",  get_capture);
	cView.define_method("parent", parent);
	cView.define_method("window", window);
	cView.define_method("make_body",   make_body);
	cView.define_method("clear_body", clear_body);
	cView.define_method("body",             body);
	cView.define_method("meter2pixel", meter2pixel);
	cView.define_method("gravity=", set_gravity);
	cView.define_method("gravity",  get_gravity);
	cView.define_method("wall", wall);
	cView.define_method("debug=", set_debug);
	cView.define_method("debug",   is_debugging);
	cView.define_method("on_attach", on_attach);
	cView.define_method("on_detach", on_detach);
	cView.define_method("on_show", on_show);
	cView.define_method("on_hide", on_hide);
	cView.define_method("on_update", on_update);
	cView.define_method("on_draw",   on_draw);
	cView.define_method("on_move",   on_move);
	cView.define_method("on_resize", on_resize);
	cView.define_method("on_rotate", on_rotate);
	cView.define_method("on_scroll", on_scroll);
	cView.define_method("on_focus", on_focus);
	cView.define_method("on_blur",  on_blur);
	cView.define_method("on_key",      on_key);
	cView.define_method("on_key_down", on_key_down);
	cView.define_method("on_key_up",   on_key_up);
	cView.define_method("on_pointer",      on_pointer);
	cView.define_method("on_pointer_down", on_pointer_down);
	cView.define_method("on_pointer_up",   on_pointer_up);
	cView.define_method("on_pointer_move", on_pointer_move);
	cView.define_method("on_wheel", on_wheel);
	cView.define_method("on_capture", on_capture);
	cView.define_method("on_timer", on_timer);
	cView.define_method("on_contact",       on_contact);
	cView.define_method("on_contact_begin", on_contact_begin);
	cView.define_method("on_contact_end",   on_contact_end);
	cView.define_const("CAPTURE_NONE",      Reflex::View::CAPTURE_NONE);
	cView.define_const("CAPTURE_KEY",       Reflex::View::CAPTURE_KEY);
	cView.define_const("CAPTURE_POINTER",   Reflex::View::CAPTURE_POINTER);
	cView.define_const("CAPTURE_ALL",       Reflex::View::CAPTURE_ALL);
	cView.define_clear_override_flags(cof);
}


namespace Reflex
{


	Class
	view_class ()
	{
		return cView;
	}


}// Reflex
