#include "reflex/ruby/style.h"


#include <rucy.h>
#include "rays/ruby/color.h"
#include "rays/ruby/image.h"
#include "reflex/ruby/selector.h"
#include "reflex/ruby/style_length.h"
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
		argc >= 1 ? (Reflex::Style::Flow) argv[0].as_i() : Reflex::Style::FLOW_DOWN,
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
RUCY_DEF1(set_size, size)
{
	CHECK;
	THIS->set_size(to<Reflex::StyleLength2>(size));
}
RUCY_END

static
RUCY_DEF0(get_size)
{
	CHECK;
	return value(THIS->size());
}
RUCY_END

static
RUCY_DEF1(set_position, position)
{
	CHECK;
	THIS->set_position(to<Reflex::StyleLength4>(position));
}
RUCY_END

static
RUCY_DEF0(get_position)
{
	CHECK;
	return value(THIS->position());
}
RUCY_END

static
RUCY_DEF1(set_margin, margin)
{
	CHECK;
	THIS->set_margin(to<Reflex::StyleLength4>(margin));
}
RUCY_END

static
RUCY_DEF0(get_margin)
{
	CHECK;
	return value(THIS->margin());
}
RUCY_END

static
RUCY_DEF1(set_padding, padding)
{
	CHECK;
	THIS->set_padding(to<Reflex::StyleLength4>(padding));
}
RUCY_END

static
RUCY_DEF0(get_padding)
{
	CHECK;
	return value(THIS->padding());
}
RUCY_END

static
RUCY_DEF1(set_background_color, color)
{
	CHECK;
	THIS->set_background_color(to<Reflex::Color&>(color));
}
RUCY_END

static
RUCY_DEF0(get_background_color)
{
	CHECK;
	return value(THIS->background_color());
}
RUCY_END

static
RUCY_DEF1(set_background_image, image)
{
	CHECK;
	THIS->set_background_image(to<Reflex::Image&>(image));
}
RUCY_END

static
RUCY_DEF0(get_background_image)
{
	CHECK;
	return THIS->background_image() ? value(THIS->background_image()) : nil();
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
	cStyle.define_method("size=", set_size);
	cStyle.define_method("size",  get_size);
	cStyle.define_method("position=", set_position);
	cStyle.define_method("position",  get_position);
	cStyle.define_method("margin=", set_margin);
	cStyle.define_method("margin",  get_margin);
	cStyle.define_method("padding=", set_padding);
	cStyle.define_method("padding",  get_padding);
	cStyle.define_method("background_color=", set_background_color);
	cStyle.define_method("background_color",  get_background_color);
	cStyle.define_method("background_image=", set_background_image);
	cStyle.define_method("background_image",  get_background_image);
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
