#include "body.h"


#include <assert.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>
#include <xot/util.h>
#include "reflex/exception.h"
#include "world.h"


namespace Reflex
{


	struct Body::Data
	{

		b2Body* b2body;

		float ppm;

		Data ()
		:	b2body(NULL), ppm(0)
		{
		}

		bool is_valid () const
		{
			return b2body && ppm > 0;
		}

	};// Body::Data


	static void
	validate (const Body* body, bool check_world_lock = false)
	{
		assert(body);

		if (!body->self->is_valid())
			invalid_state_error(__FILE__, __LINE__);

		if (check_world_lock)
		{
			assert(body->self->b2body->GetWorld());

			if (body->self->b2body->GetWorld()->IsLocked())
				physics_error(__FILE__, __LINE__);
		}
	}


	Body::Body (b2Body* b2body, float pixels_per_meter)
	{
		assert(b2body && pixels_per_meter > 0);

		self->b2body = b2body;
		self->ppm    = pixels_per_meter;
	}

	Body::~Body ()
	{
		remove_self();
	}

	void
	Body::remove_self ()
	{
		validate(this, true);

		self->b2body->GetWorld()->DestroyBody(self->b2body);

		self->b2body = NULL;
		self->ppm    = 0;
	}

	void
	Body::apply_force (coord x, coord y)
	{
		apply_force(Point(x, y));
	}

	void
	Body::apply_force (const Point& force)
	{
		validate(this);

		self->b2body->ApplyForceToCenter(to_b2vec2(force, self->ppm), true);
	}

	void
	Body::apply_torque (float torque)
	{
		validate(this);

		self->b2body->ApplyTorque(torque, true);
	}

	void
	Body::apply_linear_impulse (coord x, coord y)
	{
		apply_linear_impulse(Point(x, y));
	}

	void
	Body::apply_linear_impulse (const Point& impulse)
	{
		validate(this);

		self->b2body->ApplyLinearImpulse(
			to_b2vec2(impulse, self->ppm), self->b2body->GetWorldCenter(), true);
	}

	void
	Body::apply_angular_impulse (float impulse)
	{
		validate(this);

		self->b2body->ApplyAngularImpulse(impulse, true);
	}

	float
	Body::meter2pixel (float meter) const
	{
		return meter * self->ppm;
	}

	void
	Body::set_transform (coord x, coord y, float degree)
	{
		validate(this, true);

		self->b2body->SetTransform(
			to_b2vec2(x, y, self->ppm), Xot::deg2rad(degree));
	}

	Point
	Body::position () const
	{
		validate(this);

		return to_point(self->b2body->GetPosition(), self->ppm);
	}

	float
	Body::angle () const
	{
		validate(this);

		return Xot::rad2deg(self->b2body->GetAngle());
	}

	static bool
	is_body_dynamic (const Body* body)
	{
		assert(body);

		return body->self->b2body->GetType() == b2_dynamicBody;
	}

	void
	Body::set_dynamic (bool dynamic)
	{
		if (dynamic == is_body_dynamic(this))
			return;

		validate(this, true);

		self->b2body->SetType(dynamic ? b2_dynamicBody : b2_staticBody);
	}

	bool
	Body::is_dynamic () const
	{
		validate(this);

		return is_body_dynamic(this);
	}

	static void
	make_body_kinematic (Body* body)
	{
		if (body->self->b2body->GetType() == b2_staticBody)
			body->self->b2body->SetType(b2_kinematicBody);
	}

	void
	Body::set_linear_velocity (coord x, coord y)
	{
		set_linear_velocity(Point(x, y));
	}

	void
	Body::set_linear_velocity (const Point& velocity)
	{
		validate(this);

		make_body_kinematic(this);

		self->b2body->SetLinearVelocity(to_b2vec2(velocity, self->ppm));
	}

	Point
	Body::linear_velocity () const
	{
		validate(this);

		return to_point(self->b2body->GetLinearVelocity(), self->ppm);
	}

	void
	Body::set_angular_velocity (float velocity)
	{
		validate(this);

		make_body_kinematic(this);

		self->b2body->SetAngularVelocity(Xot::deg2rad(velocity));
	}

	float
	Body::angular_velocity () const
	{
		validate(this);

		return Xot::rad2deg(self->b2body->GetAngularVelocity());
	}

	void
	Body::set_gravity_scale (float scale)
	{
		validate(this);

		if (scale == self->b2body->GetGravityScale())
			return;

		return self->b2body->SetGravityScale(scale);
	}

	float
	Body::gravity_scale () const
	{
		validate(this);

		return self->b2body->GetGravityScale();
	}


	void
	Body_copy_attributes (const Body* from, Body* to)
	{
		if (!from || !to)
			return;

		if (from->self->ppm != to->self->ppm)
			physics_error(__FILE__, __LINE__);

		const b2Body* b2from = Body_get_b2ptr(from);
		      b2Body* b2to   = Body_get_b2ptr(to);
		assert(b2from && b2to);

		b2to->SetType(           b2from->GetType());
		b2to->SetTransform(      b2from->GetPosition(), b2from->GetAngle());
		b2to->SetLinearVelocity( b2from->GetLinearVelocity());
		b2to->SetAngularVelocity(b2from->GetAngularVelocity());
		b2to->SetLinearDamping(  b2from->GetLinearDamping());
		b2to->SetAngularDamping( b2from->GetAngularDamping());
		b2to->SetGravityScale(   b2from->GetGravityScale());
		b2to->SetBullet(         b2from->IsBullet());
	}

	Body*
	Body_create_temporary ()
	{
		return World_get_temporary()->create_body();
	}

	bool
	Body_is_temporary (const Body* body)
	{
		const b2Body* b2body = Body_get_b2ptr(body);
		if (!b2body) return false;

		return b2body->GetWorld() == World_get_b2ptr(World_get_temporary());
	}

	b2Body*
	Body_get_b2ptr (Body* body)
	{
		return body ? body->self->b2body : NULL;
	}

	const b2Body*
	Body_get_b2ptr (const Body* body)
	{
		return Body_get_b2ptr(const_cast<Body*>(body));
	}


}// Reflex
