#include "reflex/body.h"


#include <math.h>
#include <assert.h>
#include <vector>
#include <Box2D/Common/b2Math.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>
#include <Box2D/Collision/Shapes/b2ChainShape.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <xot/util.h>
#include "reflex/exception.h"
#include "reflex/painter.h"
#include "world.h"


#define PTR ((b2Body*) handle)


namespace Reflex
{


	static const float PI   = M_PI;

	static const float PI_2 = PI * 2;


	Body::Body (Handle h, float pixels_per_meter)
	:	handle(h), ppm(pixels_per_meter)
	{
		assert(h);
	}

	Fixture
	Body::add_box (coord width, coord height)
	{
		assert(PTR && PTR->GetWorld());

		if (PTR->GetWorld()->IsLocked())
			invalid_state_error(__FILE__, __LINE__);

		b2Vec2 size(width / ppm / 2, height / ppm / 2);

		b2PolygonShape shape;
		shape.SetAsBox(size.x, size.y);
		for (int i = 0; i < 4; ++i)
			shape.m_vertices[i] += size;

		b2FixtureDef fixture;
		fixture.shape = &shape;

		return PTR->CreateFixture(&fixture);
	}

	Fixture
	add_circle (b2Body* handle, coord size, float ppm)
	{
		coord radius = size / ppm / 2.f;

		b2CircleShape shape;
		shape.m_radius = radius;
		shape.m_p.Set(radius, radius);

		b2FixtureDef fixture;
		fixture.shape = &shape;

		return PTR->CreateFixture(&fixture);
	}

	Fixture
	add_ellipse (
		b2Body* handle, coord width, coord height, float from, float to,
		coord radius_min, uint nsegment, float ppm)
	{
		coord w = width / 2, h = height / 2;

		if (nsegment == 0) nsegment = Painter::ELLIPSE_NSEGMENT;

		std::vector<b2Vec2> vecs;
		vecs.reserve(nsegment);
		for (uint seg = 0; seg < nsegment; ++seg)
		{
			float pos    = (float) seg / (float) nsegment;
			float radian = (from + (to - from) * pos) * PI_2;
			float x      = cos(radian);
			float y      = -sin(radian);
			vecs.push_back(b2Vec2(to_b2coord(w + w * x, ppm), to_b2coord(h + h * y, ppm)));
		}

		b2PolygonShape shape;
		shape.Set(&vecs[0], (int32) nsegment);

		b2FixtureDef fixture;
		fixture.shape = &shape;

		return PTR->CreateFixture(&fixture);
	}

	Fixture
	Body::add_ellipse (coord width, coord height, coord radius_min, uint nsegment)
	{
		assert(PTR && PTR->GetWorld());

		if (PTR->GetWorld()->IsLocked())
			invalid_state_error(__FILE__, __LINE__);

		if (width == height && radius_min == 0)
			return add_circle(PTR, width, ppm);
		else
		{
			return Reflex::add_ellipse(
				PTR, width, height, 0, 360, radius_min, nsegment, ppm);
		}
	}

	Fixture
	Body::add_arc (
		coord width, coord height, float angle_from, float angle_to,
		coord radius_min, uint nsegment)
	{
		assert(PTR && PTR->GetWorld());

		if (PTR->GetWorld()->IsLocked())
			invalid_state_error(__FILE__, __LINE__);

		if (width == height && angle_from == 0 && angle_to == 360 && radius_min == 0)
			return add_circle(PTR, width, ppm);
		else
		{
			return Reflex::add_ellipse(
				PTR, width, height, angle_from, angle_to, radius_min, nsegment, ppm);
		}
	}

	Fixture
	Body::add_edge (const Point& begin, const Point& end)
	{
		assert(PTR && PTR->GetWorld());

		if (PTR->GetWorld()->IsLocked())
			invalid_state_error(__FILE__, __LINE__);

		b2EdgeShape shape;
		shape.Set(to_b2vec2(begin, ppm), to_b2vec2(end, ppm));

		b2FixtureDef fixture;
		fixture.shape = &shape;

		return PTR->CreateFixture(&fixture);
	}

	Fixture
	Body::add_edge (const Point* points, size_t size, bool loop)
	{
		assert(PTR && PTR->GetWorld());

		if (PTR->GetWorld()->IsLocked())
			invalid_state_error(__FILE__, __LINE__);

		if (size == 2)
			return add_edge(points[0], points[1]);

		std::vector<b2Vec2> vecs;
		vecs.reserve(size);
		for (size_t i = 0; i < size; ++i)
			vecs.push_back(to_b2vec2(points[i], ppm));

		b2ChainShape shape;
		if (loop)
			shape.CreateLoop(&vecs[0], (int32) size);
		else
			shape.CreateChain(&vecs[0], (int32) size);

		b2FixtureDef fixture;
		fixture.shape = &shape;

		return PTR->CreateFixture(&fixture);
	}

	Fixture
	Body::add_polygon (const Point* points, size_t size)
	{
		assert(PTR && PTR->GetWorld());

		if (PTR->GetWorld()->IsLocked())
			invalid_state_error(__FILE__, __LINE__);

		std::vector<b2Vec2> vecs;
		vecs.reserve(size);
		for (size_t i = 0; i < size; ++i)
			vecs.push_back(to_b2vec2(points[i], ppm));

		b2PolygonShape shape;
		shape.Set(&vecs[0], (int32) size);

		b2FixtureDef fixture;
		fixture.shape = &shape;

		return PTR->CreateFixture(&fixture);
	}

	void
	Body::clear_fixtures ()
	{
		assert(PTR && PTR->GetWorld());

		if (PTR->GetWorld()->IsLocked())
			invalid_state_error(__FILE__, __LINE__);

		b2Fixture* f;
		while ((f = PTR->GetFixtureList()))
			PTR->DestroyFixture(f);
	}

	float
	Body::meter2pixel (float meter) const
	{
		return meter * ppm;
	}

	void
	Body::set_static (bool state)
	{
		assert(PTR && PTR->GetWorld());

		if (PTR->GetWorld()->IsLocked())
			invalid_state_error(__FILE__, __LINE__);

		PTR->SetType(state ? b2_staticBody : b2_dynamicBody);
	}

	bool
	Body::is_static () const
	{
		assert(PTR);

		return PTR->GetType() == b2_staticBody;
	}

	void
	Body::set_dynamic (bool state)
	{
		assert(PTR && PTR->GetWorld());

		if (PTR->GetWorld()->IsLocked())
			invalid_state_error(__FILE__, __LINE__);

		PTR->SetType(state ? b2_dynamicBody : b2_staticBody);
	}

	bool
	Body::is_dynamic () const
	{
		assert(PTR);

		return PTR->GetType() == b2_dynamicBody;
	}

	Point
	Body::position () const
	{
		assert(PTR);

		return to_point(PTR->GetPosition(), ppm);
	}

	float
	Body::angle () const
	{
		assert(PTR);

		return Xot::rad2deg(PTR->GetAngle());
	}

	void
	Body::set_linear_velocity (coord x, coord y)
	{
		set_linear_velocity(Point(x, y));
	}

	void
	Body::set_linear_velocity (const Point& velocity)
	{
		assert(PTR);

		PTR->SetLinearVelocity(to_b2vec2(velocity, ppm));
	}

	Point
	Body::linear_velocity () const
	{
		assert(PTR);

		return to_point(PTR->GetLinearVelocity(), ppm);
	}

	void
	Body::set_angular_velocity (float velocity)
	{
		assert(PTR);

		PTR->SetAngularVelocity(Xot::deg2rad(velocity));
	}

	float
	Body::angular_velocity () const
	{
		assert(PTR);

		return Xot::rad2deg(PTR->GetAngularVelocity());
	}

	void
	Body::apply_force (coord x, coord y)
	{
		apply_force(Point(x, y));
	}

	void
	Body::apply_force (const Point& force)
	{
		assert(PTR);

		PTR->ApplyForceToCenter(to_b2vec2(force, ppm), true);
	}

	void
	Body::apply_torque (float torque)
	{
		assert(PTR);

		PTR->ApplyTorque(torque, true);
	}

	void
	Body::apply_linear_impulse (coord x, coord y)
	{
		apply_linear_impulse(Point(x, y));
	}

	void
	Body::apply_linear_impulse (const Point& impulse)
	{
		assert(PTR);

		PTR->ApplyLinearImpulse(
			to_b2vec2(impulse, ppm), PTR->GetWorldCenter(), true);
	}

	void
	Body::apply_angular_impulse (float impulse)
	{
		assert(PTR);

		PTR->ApplyAngularImpulse(impulse, true);
	}

	void
	Body::set_density (float density)
	{
		iterator end_ = end();
		for (iterator it = begin(); it != end_; ++it)
			it->set_density(density);
	}

	float
	Body::density () const
	{
		const_iterator it = begin(), end_ = end();
		if (it == end_)
			invalid_state_error(__FILE__, __LINE__, "no fixture.");

		float val = it->density();
		for (; it != end_; ++it)
		{
			if (val != it->density())
			{
				invalid_state_error(
					__FILE__, __LINE__,
					"each fixture have different values.");
			}
		}
		return val;
	}

	void
	Body::set_friction (float friction)
	{
		iterator end_ = end();
		for (iterator it = begin(); it != end_; ++it)
			it->set_friction(friction);
	}

	float
	Body::friction () const
	{
		const_iterator it = begin(), end_ = end();
		if (it == end_)
			invalid_state_error(__FILE__, __LINE__, "no fixture.");

		float val = it->friction();
		for (; it != end_; ++it)
		{
			if (val != it->friction())
			{
				invalid_state_error(
					__FILE__, __LINE__,
					"each fixture have different values.");
			}
		}
		return val;
	}

	void
	Body::set_restitution (float restitution)
	{
		iterator end_ = end();
		for (iterator it = begin(); it != end_; ++it)
			it->set_restitution(restitution);
	}

	float
	Body::restitution () const
	{
		const_iterator it = begin(), end_ = end();
		if (it == end_)
			invalid_state_error(__FILE__, __LINE__, "no fixture.");

		float val = it->restitution();
		for (; it != end_; ++it)
		{
			if (val != it->restitution())
			{
				invalid_state_error(
					__FILE__, __LINE__,
					"each fixture have different values.");
			}
		}
		return val;
	}

	void
	Body::set_sensor (bool sensor)
	{
		iterator end_ = end();
		for (iterator it = begin(); it != end_; ++it)
			it->set_sensor(sensor);
	}

	bool
	Body::is_sensor () const
	{
		const_iterator it = begin(), end_ = end();
		if (it == end_)
			invalid_state_error(__FILE__, __LINE__, "no fixture.");

		bool val = it->is_sensor();
		for (; it != end_; ++it)
		{
			if (val != it->is_sensor())
			{
				invalid_state_error(
					__FILE__, __LINE__,
					"each fixture have different values.");
			}
		}
		return val;
	}

	void
	Body::set_category (uint bits)
	{
		iterator end_ = end();
		for (iterator it = begin(); it != end_; ++it)
			it->set_category(bits);
	}

	uint
	Body::category () const
	{
		const_iterator it = begin(), end_ = end();
		if (it == end_)
			invalid_state_error(__FILE__, __LINE__, "no fixture.");

		uint val = it->category();
		for (; it != end_; ++it)
		{
			if (val != it->category())
			{
				invalid_state_error(
					__FILE__, __LINE__,
					"each fixture have different values.");
			}
		}
		return val;
	}

	void
	Body::set_collision (uint category_mask)
	{
		iterator end_ = end();
		for (iterator it = begin(); it != end_; ++it)
			it->set_collision(category_mask);
	}

	uint
	Body::collision () const
	{
		const_iterator it = begin(), end_ = end();
		if (it == end_)
			invalid_state_error(__FILE__, __LINE__, "no fixture.");

		uint val = it->collision();
		for (; it != end_; ++it)
		{
			if (val != it->collision())
			{
				invalid_state_error(
					__FILE__, __LINE__,
					"each fixture have different values.");
			}
		}
		return val;
	}

	void
	Body::set_gravity_scale (float scale)
	{
		assert(PTR);

		return PTR->SetGravityScale(scale);
	}

	float
	Body::gravity_scale () const
	{
		assert(PTR);

		return PTR->GetGravityScale();
	}

	Body::iterator
	Body::begin ()
	{
		assert(PTR);

		return Fixture(PTR->GetFixtureList());
	}

	Body::const_iterator
	Body::begin () const
	{
		assert(PTR);

		return Fixture(PTR->GetFixtureList());
	}

	Body::iterator
	Body::end ()
	{
		assert(PTR);

		return Fixture(NULL);
	}

	Body::const_iterator
	Body::end () const
	{
		assert(PTR);

		return Fixture(NULL);
	}

	void
	Body::set_transform (coord x, coord y, float degree)
	{
		assert(PTR && PTR->GetWorld());

		if (PTR->GetWorld()->IsLocked())
			invalid_state_error(__FILE__, __LINE__);

		PTR->SetTransform(to_b2vec2(x, y, ppm), Xot::deg2rad(degree));
	}


}// Reflex
