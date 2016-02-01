#include "reflex/ruby/body.h"


#include <rucy.h>
#include "rays/ruby/point.h"
#include "reflex/ruby/fixture.h"
#include "defs.h"


using namespace Rucy;

using Reflex::coord;


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
	return value(THIS->add_box(to<coord>(width), to<coord>(height)));
}
RUCY_END

static
RUCY_DEF2(add_ellipse, width, height)
{
	CHECK;
	return value(THIS->add_ellipse(to<coord>(width), to<coord>(height)));
}
RUCY_END

static
RUCY_DEF4(add_arc, width, height, angle_from, angle_to)
{
	CHECK;
	return value(THIS->add_arc(
		to<coord>(width), to<coord>(height),
		to<coord>(angle_from), to<coord>(angle_to)));
}
RUCY_END

static
RUCY_DEFN(add_edge)
{
	CHECK;
	if (argc <= 0)
		argument_error(__FILE__, __LINE__, "Body#add_edge");

	std::vector<Rays::Coord3> points;
	points.reserve(argc);
	for (int i = 0; i < argc; ++i)
		points.push_back(to<Rays::Point>(argv[i]));

	return value(THIS->add_edge((Rays::Point*) &points[0], points.size()));
}
RUCY_END

static
RUCY_DEFN(add_polygon)
{
	CHECK;
	if (argc <= 0)
		argument_error(__FILE__, __LINE__, "Body#add_polygon");

	std::vector<Rays::Coord3> points;
	points.reserve(argc);
	for (int i = 0; i < argc; ++i)
		points.push_back(to<Rays::Point>(argv[i]));

	return value(THIS->add_polygon((Rays::Point*) &points[0], points.size()));
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

	float meter = argc >= 1 ? to<float>(argv[0]) : 1;
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
RUCY_DEFN(set_linear_velocity)
{
	CHECK;
	THIS->set_linear_velocity(to<Rays::Point>(argc, argv));
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
	THIS->set_angular_velocity(to<float>(velocity));
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
RUCY_DEFN(apply_force)
{
	CHECK;
	THIS->apply_force(to<Rays::Point>(argc, argv));
	return self;
}
RUCY_END

static
RUCY_DEF1(apply_torque, torque)
{
	CHECK;
	THIS->apply_torque(to<float>(torque));
	return self;
}
RUCY_END

static
RUCY_DEFN(apply_linear_impulse)
{
	CHECK;
	THIS->apply_linear_impulse(to<Rays::Point>(argc, argv));
	return self;
}
RUCY_END

static
RUCY_DEF1(apply_angular_impulse, impulse)
{
	CHECK;
	THIS->apply_angular_impulse(to<float>(impulse));
	return self;
}
RUCY_END

static
RUCY_DEF1(set_density, density)
{
	CHECK;
	THIS->set_density(to<float>(density));
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
	THIS->set_friction(to<float>(friction));
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
	THIS->set_restitution(to<float>(restitution));
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
RUCY_DEF1(set_sensor, sensor)
{
	CHECK;
	THIS->set_sensor(sensor);
	return self;
}
RUCY_END

static
RUCY_DEF0(get_sensor)
{
	CHECK;
	return value(THIS->is_sensor());
}
RUCY_END

static
RUCY_DEF1(set_category, bits)
{
	CHECK;
	THIS->set_category(bits.as_i());
	return self;
}
RUCY_END

static
RUCY_DEF0(get_category)
{
	CHECK;
	return value(THIS->category());
}
RUCY_END

static
RUCY_DEF1(set_collision, category_mask)
{
	CHECK;
	THIS->set_collision(category_mask.as_i());
	return self;
}
RUCY_END

static
RUCY_DEF0(get_collision)
{
	CHECK;
	return value(THIS->collision());
}
RUCY_END

static
RUCY_DEF1(set_gravity_scale, scale)
{
	CHECK;
	THIS->set_gravity_scale(to<float>(scale));
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
	cBody.define_method("add_edge",    add_edge);
	cBody.define_method("add_polygon", add_polygon);
	cBody.define_method("clear_fixtures", clear_fixtures);
	cBody.define_method("meter2pixel", meter2pixel);
	cBody.define_method("static=", set_static);
	cBody.define_method("static",   is_static);
	cBody.define_method("dynamic=", set_dynamic);
	cBody.define_method("dynamic",   is_dynamic);
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
	cBody.define_method("sensor=",    set_sensor);
	cBody.define_method("sensor",     get_sensor);
	cBody.define_method("category=",  set_category);
	cBody.define_method("category",   get_category);
	cBody.define_method("collision=", set_collision);
	cBody.define_method("collision",  get_collision);
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
