#include "reflex/ruby/style.h"


#include <rucy.h>
#include "rays/ruby/color.h"
#include "rays/ruby/image.h"
#include "reflex/ruby/selector.h"
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_VALUE_FROM_TO(Reflex::Style)

#define THIS  to<Reflex::Style*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::Style, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Reflex::Style>(klass);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::Style&>(obj);
	return self;
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
RUCY_DEF0(each_tag)
{
	CHECK;

	Value ret;
	Reflex::Style::tag_iterator end = THIS->tag_end();
	for (Reflex::Style::tag_iterator it = THIS->tag_begin(); it != end; ++it)
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
	return value(THIS->selector());
}
RUCY_END

static
RUCY_DEFN(set_flow)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Style#set_flow", argc, 1, 2);

	THIS->set_flow(
		(Reflex::Style::Flow) argv[0].as_i(),
		argc >= 2 ? (Reflex::Style::Flow) argv[1].as_i() : Reflex::Style::FLOW_NONE);
}
RUCY_END

static
RUCY_DEF0(get_flow)
{
	CHECK;
	Reflex::Style::Flow main, sub;
	THIS->get_flow(&main, &sub);
	return array(main, sub);
}
RUCY_END

static
RUCY_DEF1(set_width, width)
{
	CHECK;
	THIS->set_width(to<Reflex::StyleLength>(width));
}
RUCY_END

static
RUCY_DEF0(get_width)
{
	CHECK;
	return value(THIS->width());
}
RUCY_END

static
RUCY_DEF1(set_height, height)
{
	CHECK;
	THIS->set_height(to<Reflex::StyleLength>(height));
}
RUCY_END

static
RUCY_DEF0(get_height)
{
	CHECK;
	return value(THIS->height());
}
RUCY_END

static
RUCY_DEFN(set_size)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Style#set_size", argc, 1, 2);

	set_width (self, argv[0]);
	set_height(self, argc >= 2 ? argv[1] : argv[0]);
}
RUCY_END

static
RUCY_DEF0(get_size)
{
	CHECK;
	return array(get_width(self), get_height(self));
}
RUCY_END

static
RUCY_DEF1(set_left, left)
{
	CHECK;
	THIS->set_left(to<Reflex::StyleLength>(left));
}
RUCY_END

static
RUCY_DEF0(get_left)
{
	CHECK;
	return value(THIS->left());
}
RUCY_END

static
RUCY_DEF1(set_top, top)
{
	CHECK;
	THIS->set_top(to<Reflex::StyleLength>(top));
}
RUCY_END

static
RUCY_DEF0(get_top)
{
	CHECK;
	return value(THIS->top());
}
RUCY_END

static
RUCY_DEF1(set_right, right)
{
	CHECK;
	THIS->set_right(to<Reflex::StyleLength>(right));
}
RUCY_END

static
RUCY_DEF0(get_right)
{
	CHECK;
	return value(THIS->right());
}
RUCY_END

static
RUCY_DEF1(set_bottom, bottom)
{
	CHECK;
	THIS->set_bottom(to<Reflex::StyleLength>(bottom));
}
RUCY_END

static
RUCY_DEF0(get_bottom)
{
	CHECK;
	return value(THIS->bottom());
}
RUCY_END

static
RUCY_DEF4(set_position, left, top, right, bottom)
{
	CHECK;
	set_left  (self, left);
	set_top   (self, top);
	set_right (self, right);
	set_bottom(self, bottom);
}
RUCY_END

static
RUCY_DEF0(get_position)
{
	CHECK;
	return array(
		get_left(self),
		get_top(self),
		get_right(self),
		get_bottom(self));
}
RUCY_END

static
RUCY_DEF1(set_offset_left, left)
{
	CHECK;
	THIS->set_offset_left(to<Reflex::StyleLength>(left));
}
RUCY_END

static
RUCY_DEF0(get_offset_left)
{
	CHECK;
	return value(THIS->offset_left());
}
RUCY_END

static
RUCY_DEF1(set_offset_top, top)
{
	CHECK;
	THIS->set_offset_top(to<Reflex::StyleLength>(top));
}
RUCY_END

static
RUCY_DEF0(get_offset_top)
{
	CHECK;
	return value(THIS->offset_top());
}
RUCY_END

static
RUCY_DEF1(set_offset_right, right)
{
	CHECK;
	THIS->set_offset_right(to<Reflex::StyleLength>(right));
}
RUCY_END

static
RUCY_DEF0(get_offset_right)
{
	CHECK;
	return value(THIS->offset_right());
}
RUCY_END

static
RUCY_DEF1(set_offset_bottom, bottom)
{
	CHECK;
	THIS->set_offset_bottom(to<Reflex::StyleLength>(bottom));
}
RUCY_END

static
RUCY_DEF0(get_offset_bottom)
{
	CHECK;
	return value(THIS->offset_bottom());
}
RUCY_END

static
RUCY_DEF4(set_offset, left, top, right, bottom)
{
	CHECK;
	set_offset_left  (self, left);
	set_offset_top   (self, top);
	set_offset_right (self, right);
	set_offset_bottom(self, bottom);
}
RUCY_END

static
RUCY_DEF0(get_offset)
{
	CHECK;
	return array(
		get_offset_left(self),
		get_offset_top(self),
		get_offset_right(self),
		get_offset_bottom(self));
}
RUCY_END

static
RUCY_DEF1(set_margin_left, left)
{
	CHECK;
	THIS->set_margin_left(to<Reflex::StyleLength>(left));
}
RUCY_END

static
RUCY_DEF0(get_margin_left)
{
	CHECK;
	return value(THIS->margin_left());
}
RUCY_END

static
RUCY_DEF1(set_margin_top, top)
{
	CHECK;
	THIS->set_margin_top(to<Reflex::StyleLength>(top));
}
RUCY_END

static
RUCY_DEF0(get_margin_top)
{
	CHECK;
	return value(THIS->margin_top());
}
RUCY_END

static
RUCY_DEF1(set_margin_right, right)
{
	CHECK;
	THIS->set_margin_right(to<Reflex::StyleLength>(right));
}
RUCY_END

static
RUCY_DEF0(get_margin_right)
{
	CHECK;
	return value(THIS->margin_right());
}
RUCY_END

static
RUCY_DEF1(set_margin_bottom, bottom)
{
	CHECK;
	THIS->set_margin_bottom(to<Reflex::StyleLength>(bottom));
}
RUCY_END

static
RUCY_DEF0(get_margin_bottom)
{
	CHECK;
	return value(THIS->margin_bottom());
}
RUCY_END

static
RUCY_DEF4(set_margin, left, top, right, bottom)
{
	CHECK;
	set_margin_left  (self, left);
	set_margin_top   (self, top);
	set_margin_right (self, right);
	set_margin_bottom(self, bottom);
}
RUCY_END

static
RUCY_DEF0(get_margin)
{
	CHECK;
	return array(
		get_margin_left(self),
		get_margin_top(self),
		get_margin_right(self),
		get_margin_bottom(self));
}
RUCY_END

static
RUCY_DEF1(set_padding_left, left)
{
	CHECK;
	THIS->set_padding_left(to<Reflex::StyleLength>(left));
}
RUCY_END

static
RUCY_DEF0(get_padding_left)
{
	CHECK;
	return value(THIS->padding_left());
}
RUCY_END

static
RUCY_DEF1(set_padding_top, top)
{
	CHECK;
	THIS->set_padding_top(to<Reflex::StyleLength>(top));
}
RUCY_END

static
RUCY_DEF0(get_padding_top)
{
	CHECK;
	return value(THIS->padding_top());
}
RUCY_END

static
RUCY_DEF1(set_padding_right, right)
{
	CHECK;
	THIS->set_padding_right(to<Reflex::StyleLength>(right));
}
RUCY_END

static
RUCY_DEF0(get_padding_right)
{
	CHECK;
	return value(THIS->padding_right());
}
RUCY_END

static
RUCY_DEF1(set_padding_bottom, bottom)
{
	CHECK;
	THIS->set_padding_bottom(to<Reflex::StyleLength>(bottom));
}
RUCY_END

static
RUCY_DEF0(get_padding_bottom)
{
	CHECK;
	return value(THIS->padding_bottom());
}
RUCY_END

static
RUCY_DEF4(set_padding, left, top, right, bottom)
{
	CHECK;
	set_padding_left  (self, left);
	set_padding_top   (self, top);
	set_padding_right (self, right);
	set_padding_bottom(self, bottom);
}
RUCY_END

static
RUCY_DEF0(get_padding)
{
	CHECK;
	return array(
		get_padding_left(self),
		get_padding_top(self),
		get_padding_right(self),
		get_padding_bottom(self));
}
RUCY_END

static
RUCY_DEFN(set_fill)
{
	CHECK;
	THIS->set_fill(to<Reflex::Color>(argc, argv));
}
RUCY_END

static
RUCY_DEF0(get_fill)
{
	CHECK;
	return value(THIS->fill());
}
RUCY_END

static
RUCY_DEFN(set_stroke)
{
	CHECK;
	THIS->set_stroke(to<Reflex::Color>(argc, argv));
}
RUCY_END

static
RUCY_DEF0(get_stroke)
{
	CHECK;
	return value(THIS->stroke());
}
RUCY_END

static
RUCY_DEF1(set_image, image)
{
	CHECK;
	THIS->set_image(to<Reflex::Image&>(image));
}
RUCY_END

static
RUCY_DEF0(get_image)
{
	CHECK;
	return THIS->image() ? value(THIS->image()) : nil();
}
RUCY_END

static
RUCY_DEF1(equal, style)
{
	CHECK;
	return value(*THIS == to<Reflex::Style&>(style));
}
RUCY_END


static Class cStyle;

void
Init_style ()
{
	Module mReflex = define_module("Reflex");

	cStyle = mReflex.define_class("Style");
	cStyle.define_alloc_func(alloc);
	cStyle.define_private_method("initialize_copy", initialize_copy);

	cStyle.define_method("name=", set_name);
	cStyle.define_method("name",  get_name);
	cStyle.define_method("add_tag",    add_tag);
	cStyle.define_method("remove_tag", remove_tag);
	cStyle.define_method("each_tag", each_tag);
	cStyle.define_method("selector=", set_selector);
	cStyle.define_method("selector", get_selector);

	cStyle.define_method("set_flow", set_flow);
	cStyle.define_method("get_flow", get_flow);

	cStyle.define_method("width=",  set_width);
	cStyle.define_method("width",   get_width);
	cStyle.define_method("height=", set_height);
	cStyle.define_method("height",  get_height);
	cStyle.define_method("size=",   set_size);
	cStyle.define_method("size",    get_size);

	cStyle.define_method("left=",     set_left);
	cStyle.define_method("left",      get_left);
	cStyle.define_method("top=",      set_top);
	cStyle.define_method("top",       get_top);
	cStyle.define_method("right=",    set_right);
	cStyle.define_method("right",     get_right);
	cStyle.define_method("bottom=",   set_bottom);
	cStyle.define_method("bottom",    get_bottom);
	cStyle.define_method("position=", set_position);
	cStyle.define_method("position",  get_position);

	cStyle.define_method("offset_left=",   set_offset_left);
	cStyle.define_method("offset_left",    get_offset_left);
	cStyle.define_method("offset_top=",    set_offset_top);
	cStyle.define_method("offset_top",     get_offset_top);
	cStyle.define_method("offset_right=",  set_offset_right);
	cStyle.define_method("offset_right",   get_offset_right);
	cStyle.define_method("offset_bottom=", set_offset_bottom);
	cStyle.define_method("offset_bottom",  get_offset_bottom);
	cStyle.define_method("offset=",        set_offset);
	cStyle.define_method("offset",         get_offset);

	cStyle.define_method("margin_left=",   set_margin_left);
	cStyle.define_method("margin_left",    get_margin_left);
	cStyle.define_method("margin_top=",    set_margin_top);
	cStyle.define_method("margin_top",     get_margin_top);
	cStyle.define_method("margin_right=",  set_margin_right);
	cStyle.define_method("margin_right",   get_margin_right);
	cStyle.define_method("margin_bottom=", set_margin_bottom);
	cStyle.define_method("margin_bottom",  get_margin_bottom);
	cStyle.define_method("margin=",        set_margin);
	cStyle.define_method("margin",         get_margin);

	cStyle.define_method("padding_left=",   set_padding_left);
	cStyle.define_method("padding_left",    get_padding_left);
	cStyle.define_method("padding_top=",    set_padding_top);
	cStyle.define_method("padding_top",     get_padding_top);
	cStyle.define_method("padding_right=",  set_padding_right);
	cStyle.define_method("padding_right",   get_padding_right);
	cStyle.define_method("padding_bottom=", set_padding_bottom);
	cStyle.define_method("padding_bottom",  get_padding_bottom);
	cStyle.define_method("padding=",        set_padding);
	cStyle.define_method("padding",         get_padding);

	cStyle.define_method("fill=",   set_fill);
	cStyle.define_method("fill",    get_fill);
	cStyle.define_method("stroke=", set_stroke);
	cStyle.define_method("stroke",  get_stroke);

	cStyle.define_method("image=", set_image);
	cStyle.define_method("image",  get_image);

	cStyle.define_method("==", equal);

	cStyle.define_const("FLOW_NONE",  Reflex::Style::FLOW_NONE);
	cStyle.define_const("FLOW_DOWN",  Reflex::Style::FLOW_DOWN);
	cStyle.define_const("FLOW_RIGHT", Reflex::Style::FLOW_RIGHT);
	cStyle.define_const("FLOW_UP",    Reflex::Style::FLOW_UP);
	cStyle.define_const("FLOW_LEFT",  Reflex::Style::FLOW_LEFT);
}


namespace Reflex
{


	Class
	style_class ()
	{
		return cStyle;
	}


}// Reflex
