#include "reflex/ruby/body.h"


#include <rucy.h>
#include "rays/ruby/point.h"
#include "reflex/ruby/fixture.h"
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_VALUE_FROM_TO(Reflex::Body)

#define THIS  to<Reflex::Body*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::Body, self)


static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::Body&>(obj);
	return self;
}
RUCY_END

static
RUCY_DEF2(add_box, width, height)
{
	CHECK;
	return value(THIS->add_box(width.as_f(true), height.as_f(true)));
}
RUCY_END

static
RUCY_DEF2(add_ellipse, width, height)
{
	CHECK;
	return value(THIS->add_ellipse(width.as_f(true), height.as_f(true)));
}
RUCY_END

static
RUCY_DEF4(add_arc, width, height, angle_from, angle_to)
{
	CHECK;
	return value(THIS->add_arc(
		width.as_f(true), height.as_f(true),
		angle_from.as_f(true), angle_to.as_f(true)));
}
RUCY_END

static
RUCY_DEF0(clear_fixtures)
{
	CHECK;
	THIS->clear_fixtures();
	return self;
}
RUCY_END

static
RUCY_DEFN(meter2pixel)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "View#meter2pixel", argc, 0, 1);

	float meter = argc >= 1 ? argv[0].as_f(true) : 1;
	return value(THIS->meter2pixel(meter));
}
RUCY_END

static
RUCY_DEF1(set_static, state)
{
	CHECK;
	THIS->set_static(state);
	return state;
}
RUCY_END

static
RUCY_DEF0(is_static)
{
	CHECK;
	return value(THIS->is_static());
}
RUCY_END

static
RUCY_DEF1(set_dynamic, state)
{
	CHECK;
	THIS->set_dynamic(state);
	return state;
}
RUCY_END

static
RUCY_DEF0(is_dynamic)
{
	CHECK;
	return value(THIS->is_dynamic());
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
RUCY_DEF0(get_angle)
{
	CHECK;
	return value(THIS->angle());
}
RUCY_END

static
RUCY_DEF1(set_linear_velocity, velocity)
{
	CHECK;
	THIS->set_linear_velocity(to<Rays::Point&>(velocity));
	return self;
}
RUCY_END

static
RUCY_DEF0(get_linear_velocity)
{
	CHECK;
	return value(THIS->linear_velocity());
}
RUCY_END

static
RUCY_DEF1(set_angular_velocity, velocity)
{
	CHECK;
	THIS->set_angular_velocity(velocity.as_f(true));
	return self;
}
RUCY_END

static
RUCY_DEF0(get_angular_velocity)
{
	CHECK;
	return value(THIS->angular_velocity());
}
RUCY_END

static
RUCY_DEF1(apply_force, force)
{
	CHECK;
	THIS->apply_force(to<Rays::Point&>(force));
	return self;
}
RUCY_END

static
RUCY_DEF1(apply_torque, torque)
{
	CHECK;
	THIS->apply_torque(torque.as_f(true));
	return self;
}
RUCY_END

static
RUCY_DEF1(apply_linear_impulse, impulse)
{
	CHECK;
	THIS->apply_linear_impulse(to<Rays::Point&>(impulse));
	return self;
}
RUCY_END

static
RUCY_DEF1(apply_angular_impulse, impulse)
{
	CHECK;
	THIS->apply_angular_impulse(impulse.as_f(true));
	return self;
}
RUCY_END

static
RUCY_DEF1(set_density, density)
{
	CHECK;
	THIS->set_density(density.as_f(true));
	return self;
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
	THIS->set_friction(friction.as_f(true));
	return self;
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
	THIS->set_restitution(restitution.as_f(true));
	return self;
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
RUCY_DEF1(set_gravity_scale, scale)
{
	CHECK;
	THIS->set_gravity_scale(scale.as_f(true));
	return self;
}
RUCY_END

static
RUCY_DEF0(get_gravity_scale)
{
	CHECK;
	return value(THIS->gravity_scale());
}
RUCY_END

static
RUCY_DEF0(each)
{
	CHECK;

	Value ret;
	Reflex::Body::iterator end = THIS->end();
	for (Reflex::Body::iterator it = THIS->begin(); it != end; ++it)
		ret = rb_yield(value(*it));
	return ret;
}
RUCY_END


static Class cBody;

void
Init_body ()
{
	Module mReflex = define_module("Reflex");

	cBody = mReflex.define_class("Body");
	cBody.define_private_method("initialize_copy", initialize_copy);
	cBody.define_method("add_box",     add_box);
	cBody.define_method("add_ellipse", add_ellipse);
	cBody.define_method("add_arc",     add_arc);
	cBody.define_method("clear_fixtures", clear_fixtures);
	cBody.define_method("meter2pixel", meter2pixel);
	cBody.define_method("static=", set_static);
	cBody.define_method("static?",  is_static);
	cBody.define_method("dynamic=", set_dynamic);
	cBody.define_method("dynamic?",  is_dynamic);
	cBody.define_method("position", get_position);
	cBody.define_method("angle",    get_angle);
	cBody.define_method("linear_velocity=",  set_linear_velocity);
	cBody.define_method("linear_velocity",   get_linear_velocity);
	cBody.define_method("angular_velocity=", set_angular_velocity);
	cBody.define_method("angular_velocity",  get_angular_velocity);
	cBody.define_method("apply_force",  apply_force);
	cBody.define_method("apply_torque", apply_torque);
	cBody.define_method("apply_linear_impulse",  apply_linear_impulse);
	cBody.define_method("apply_angular_impulse", apply_angular_impulse);
	cBody.define_method("density=",     set_density);
	cBody.define_method("density",      get_density);
	cBody.define_method("friction=",    set_friction);
	cBody.define_method("friction",     get_friction);
	cBody.define_method("restitution=", set_restitution);
	cBody.define_method("restitution",  get_restitution);
	cBody.define_method("gravity_scale=", set_gravity_scale);
	cBody.define_method("gravity_scale",  get_gravity_scale);
	cBody.define_method("each", each);
}


namespace Reflex
{


	Class
	body_class ()
	{
		return cBody;
	}


}// Reflex
