#include "reflex/fixture.h"


#include <assert.h>
#include <limits.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include "reflex/exception.h"


#define PTR ((b2Fixture*) handle)


namespace Reflex
{


	Fixture::Fixture (Handle h)
	:	handle(h)
	{
	}

	Fixture&
	Fixture::operator = (Handle h)
	{
		handle = h;
		return *this;
	}

	void
	Fixture::set_density (float density)
	{
		assert(PTR && PTR->GetBody());

		PTR->SetDensity(density);
		PTR->GetBody()->ResetMassData();
	}

	float
	Fixture::density () const
	{
		assert(PTR);

		return PTR->GetDensity();
	}

	void
	Fixture::set_friction (float friction)
	{
		assert(PTR);

		PTR->SetFriction(friction);
	}

	float
	Fixture::friction () const
	{
		assert(PTR);

		return PTR->GetFriction();
	}

	void
	Fixture::set_restitution (float restitution)
	{
		assert(PTR);

		PTR->SetRestitution(restitution);
	}

	float
	Fixture::restitution () const
	{
		assert(PTR);

		return PTR->GetRestitution();
	}

	void
	Fixture::set_sensor (bool sensor)
	{
		assert(PTR);

		PTR->SetSensor(sensor);
	}

	bool
	Fixture::is_sensor () const
	{
		assert(PTR);

		return PTR->IsSensor();
	}

	void
	Fixture::set_category (uint bits)
	{
		assert(PTR);

		if (bits > USHRT_MAX)
			argument_error(__FILE__, __LINE__, "category value must be less then USHRT_MAX.");

		b2Filter f = PTR->GetFilterData();
		f.categoryBits = bits;
		PTR->SetFilterData(f);
	}

	uint
	Fixture::category () const
	{
		assert(PTR);

		return PTR->GetFilterData().categoryBits;
	}

	void
	Fixture::set_collision (uint category_mask)
	{
		assert(PTR);

		if (category_mask > USHRT_MAX)
			argument_error(__FILE__, __LINE__, "category mask value must be less then USHRT_MAX.");

		b2Filter f = PTR->GetFilterData();
		f.maskBits = category_mask;
		PTR->SetFilterData(f);
	}

	uint
	Fixture::collision () const
	{
		assert(PTR);

		return PTR->GetFilterData().maskBits;
	}

	Fixture::Handle
	Fixture::next () const
	{
		assert(PTR);

		return PTR->GetNext();
	}

	Fixture::operator bool () const
	{
		return PTR;
	}

	bool
	Fixture::operator ! () const
	{
		return !operator bool();
	}

	bool
	operator == (const Fixture& lhs, const Fixture& rhs)
	{
		return lhs.handle == rhs.handle;
	}

	bool
	operator != (const Fixture& lhs, const Fixture& rhs)
	{
		return !(lhs == rhs);
	}


}// Reflex
