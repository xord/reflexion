#include "reflex/ruby/timer.h"


#include <rucy.h>
#include "reflex/ruby/view.h"
#include "reflex/ruby/selector.h"
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_WRAPPER_VALUE_FROM_TO(Reflex::Timer)

#define   THIS to<      Reflex::Timer*>(self)
#define C_THIS to<const Reflex::Timer*>(self)

#define CHECK  RUCY_CHECK_OBJ(Reflex::Timer, self)


static
RUCY_DEF0(get_owner)
{
	CHECK;
	return value(THIS->owner());
}
RUCY_END

static
RUCY_DEF0(get_id)
{
	CHECK;
	return value(THIS->id());
}
RUCY_END

static
RUCY_DEF0(get_interval)
{
	CHECK;
	return value(THIS->interval());
}
RUCY_END

static
RUCY_DEF1(set_count, count)
{
	CHECK;
	THIS->set_count(count.as_i(true));
	return count;
}
RUCY_END

static
RUCY_DEF0(get_count)
{
	CHECK;
	return value(THIS->count());
}
RUCY_END

static
RUCY_DEF0(is_finished)
{
	CHECK;
	return value(THIS->is_finished());
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
	return C_THIS->name() ? value(C_THIS->name()) : nil();
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
	Reflex::Timer::const_tag_iterator end = C_THIS->tag_end();
	for (Reflex::Timer::const_tag_iterator it = C_THIS->tag_begin(); it != end; ++it)
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
RUCY_DEF_clear_override_flags(cof, Reflex::Timer);


static Class cTimer;

void
Init_timer ()
{
	Module mReflex = define_module("Reflex");

	cTimer = mReflex.define_class("Timer");

	cTimer.define_method("owner", get_owner);
	cTimer.define_method("id", get_id);
	cTimer.define_method("interval", get_interval);
	cTimer.define_method("count=", set_count);
	cTimer.define_method("count",  get_count);
	cTimer.define_method("finish?", is_finished);

	cTimer.define_method("name=", set_name);
	cTimer.define_method("name",  get_name);
	cTimer.define_method("add_tag",    add_tag);
	cTimer.define_method("remove_tag", remove_tag);
	cTimer.define_method("each_tag", each_tag);
	cTimer.define_method("selector=", set_selector);
	cTimer.define_method("selector", get_selector);

	cTimer.define_clear_override_flags(cof);
}


namespace Reflex
{


	Class
	timer_class ()
	{
		return cTimer;
	}


}// Reflex
