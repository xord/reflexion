#include "reflex/ruby/fixture.h"


#include <rucy.h>
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_VALUE_FROM_TO(Reflex::Fixture)

#define THIS  to<Reflex::Fixture*>(self)

#define CHECK RUCY_CHECK_OBJ(Reflex::Fixture, self)


static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Reflex::Fixture&>(obj);
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
	THIS->set_category(bits.as_i(true));
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
	THIS->set_collision(category_mask.as_i(true));
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


static Class cFixture;

void
Init_fixture ()
{
	Module mReflex = define_module("Reflex");

	cFixture = mReflex.define_class("Fixture");
	cFixture.define_private_method("initialize_copy", initialize_copy);
	cFixture.define_method("density=",     set_density);
	cFixture.define_method("density",      get_density);
	cFixture.define_method("friction=",    set_friction);
	cFixture.define_method("friction",     get_friction);
	cFixture.define_method("restitution=", set_restitution);
	cFixture.define_method("restitution",  get_restitution);
	cFixture.define_method("sensor=",    set_sensor);
	cFixture.define_method("sensor",     get_sensor);
	cFixture.define_method("category=",  set_category);
	cFixture.define_method("category",   get_category);
	cFixture.define_method("collision=", set_collision);
	cFixture.define_method("collision",  get_collision);
}


namespace Reflex
{


	Class
	fixture_class ()
	{
		return cFixture;
	}


}// Reflex
