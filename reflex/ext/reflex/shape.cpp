#include "reflex/ruby/shape.h"


#include <rays/ruby/bounds.h>
#include "reflex/exception.h"
#include "reflex/ruby/view.h"
#include "defs.h"
#include "selector.h"


RUCY_DEFINE_WRAPPER_VALUE_FROM_TO(Reflex::Shape)

#define THIS      to<Reflex::Shape*>(self)

#define CHECK     RUCY_CHECK_OBJ(Reflex::Shape, self)

#define CALL(fun) RUCY_CALL_SUPER(THIS, fun)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	Reflex::reflex_error(__FILE__, __LINE__, "can not instantiate Shape class.");
}
RUCY_END

static
RUCY_DEF0(owner)
{
	CHECK;
	return value(THIS->owner());
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
RUCY_DEF1(set_density, density)
{
	CHECK;
	THIS->set_density(to<float>(density));
	return density;
}
RUCY_END

static
RUCY_DEF0(get_density)
{
	CHECK;
	return value(THIS->density());
}
RUCY_END

static
RUCY_DEF1(set_friction, friction)
{
	CHECK;
	THIS->set_friction(to<float>(friction));
	return friction;
}
RUCY_END

static
RUCY_DEF0(get_friction)
{
	CHECK;
	return value(THIS->friction());
}
RUCY_END

static
RUCY_DEF1(set_restitution, restitution)
{
	CHECK;
	THIS->set_restitution(to<float>(restitution));
	return restitution;
}
RUCY_END

static
RUCY_DEF0(get_restitution)
{
	CHECK;
	return value(THIS->restitution());
}
RUCY_END

static
RUCY_DEF1(set_sensor, state)
{
	CHECK;
	THIS->set_sensor(state);
	return state;
}
RUCY_END

static
RUCY_DEF0(is_sensor)
{
	CHECK;
	return value(THIS->is_sensor());
}
RUCY_END

static
RUCY_DEF1(set_category_bits, bits)
{
	CHECK;
	THIS->set_category_bits(to<uint>(bits));
	return bits;
}
RUCY_END

static
RUCY_DEF0(get_category_bits)
{
	CHECK;
	return value(THIS->category_bits());
}
RUCY_END

static
RUCY_DEF1(set_collision_mask, mask)
{
	CHECK;
	THIS->set_collision_mask(to<uint>(mask));
	return mask;
}
RUCY_END

static
RUCY_DEF0(get_collision_mask)
{
	CHECK;
	return value(THIS->collision_mask());
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


static Class cShape;

void
Init_shape ()
{
	Module mReflex = define_module("Reflex");

	cShape = mReflex.define_class("Shape");
	cShape.define_alloc_func(alloc);
	cShape.define_method("owner", owner);
	cShape.define_method("frame=", set_frame);
	cShape.define_method("frame",  get_frame);
	cShape.define_method("density=",     set_density);
	cShape.define_method("density",      get_density);
	cShape.define_method("friction=",    set_friction);
	cShape.define_method("friction",     get_friction);
	cShape.define_method("restitution=", set_restitution);
	cShape.define_method("restitution",  get_restitution);
	cShape.define_method("sensor=",      set_sensor);
	cShape.define_method("sensor",        is_sensor);
	cShape.define_method("category_bits=",  set_category_bits);
	cShape.define_method("category_bits",   get_category_bits);
	cShape.define_method("collision_mask=", set_collision_mask);
	cShape.define_method("collision_mask",  get_collision_mask);
	cShape.define_method("on_draw",          on_draw);
	cShape.define_private_method("call_contact!",       on_contact);
	cShape.define_private_method("call_contact_begin!", on_contact_begin);
	cShape.define_private_method("call_contact_end!",   on_contact_end);

	define_selector_methods<Reflex::Shape>(cShape);
}


namespace Reflex
{


	Class
	shape_class ()
	{
		return cShape;
	}


}// Reflex
