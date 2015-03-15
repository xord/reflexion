#include "reflex/fixture.h"


#include <assert.h>
#include <Box2D/Dynamics/b2Fixture.h>


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
