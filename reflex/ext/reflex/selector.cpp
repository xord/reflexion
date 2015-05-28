#include "reflex/ruby/selector.h"


#include <rucy.h>
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_VALUE_FROM_TO(Reflex::Selector)

#define THIS  to<Reflex::Selector*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::Selector, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Reflex::Selector>(klass);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::Selector&>(obj);
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
	Reflex::Selector::iterator end = THIS->end();
	for (Reflex::Selector::iterator it = THIS->begin(); it != end; ++it)
		ret = rb_yield(value(*it));
	return ret;
}
RUCY_END

static
RUCY_DEF1(contains, selector)
{
	CHECK;
	return value(THIS->contains(to<Reflex::Selector&>(selector)));
}
RUCY_END

static
RUCY_DEF1(equal, selector)
{
	CHECK;
	return value(*THIS == to<Reflex::Selector&>(selector));
}
RUCY_END


static Class cSelector;

void
Init_selector ()
{
	Module mReflex = define_module("Reflex");

	cSelector = mReflex.define_class("Selector");
	cSelector.define_alloc_func(alloc);
	cSelector.define_private_method("initialize_copy", initialize_copy);
	cSelector.define_method("name=", set_name);
	cSelector.define_method("name",  get_name);
	cSelector.define_method("add_tag",    add_tag);
	cSelector.define_method("remove_tag", remove_tag);
	cSelector.define_method("each_tag", each_tag);
	cSelector.define_method("contains", contains);
	cSelector.define_method("==", equal);
}


namespace Rucy
{


	template <> Reflex::Selector
	value_to<Reflex::Selector> (Value value, bool convert)
	{
		if (convert && (value.is_s() || value.is_sym()))
			return Reflex::Selector(value.c_str());

		return value_to<Reflex::Selector&>(value, convert);
	}


}// Rucy


namespace Reflex
{


	Class
	selector_class ()
	{
		return cSelector;
	}


}// Reflex
