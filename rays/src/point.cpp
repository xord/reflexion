#include "point.h"


#include <math.h>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <xot/string.h>
#include <xot/util.h>
#include "rays/exception.h"


namespace Rays
{


	Coord2&
	Coord2::reset (coord value)
	{
		return reset(value, value);
	}

	Coord2&
	Coord2::reset (coord x, coord y)
	{
		this->x = x;
		this->y = y;
		return *this;
	}

	size_t
	Coord2::size () const
	{
		return SIZE;
	}

	coord&
	Coord2::operator [] (size_t index)
	{
		if (index >= 2)
			argument_error(__FILE__, __LINE__);

		return array[index];
	}

	const coord&
	Coord2::operator [] (size_t index) const
	{
		return const_cast<Coord2*>(this)->operator[](index);
	}


	Coord3&
	Coord3::operator = (const Coord2& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = 0;
		return *this;
	}

	Coord3&
	Coord3::reset (coord value)
	{
		return reset(value, value);
	}

	Coord3&
	Coord3::reset (coord x, coord y, coord z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		return *this;
	}

	size_t
	Coord3::size () const
	{
		return SIZE;
	}

	coord&
	Coord3::operator [] (size_t index)
	{
		if (index >= 3)
			argument_error(__FILE__, __LINE__);

		return array[index];
	}

	const coord&
	Coord3::operator [] (size_t index) const
	{
		return const_cast<Coord3*>(this)->operator[](index);
	}


	Coord4&
	Coord4::operator = (const Coord2& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = 0;
		w = 1;
		return *this;
	}

	Coord4&
	Coord4::operator = (const Coord3& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = 1;
		return *this;
	}

	Coord4&
	Coord4::reset (coord value)
	{
		return reset(value, value, value);
	}

	Coord4&
	Coord4::reset (coord x, coord y, coord z, coord w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
		return *this;
	}

	size_t
	Coord4::size () const
	{
		return SIZE;
	}

	coord&
	Coord4::operator [] (size_t index)
	{
		if (index >= 4)
			argument_error(__FILE__, __LINE__);

		return array[index];
	}

	const coord&
	Coord4::operator [] (size_t index) const
	{
		return const_cast<Coord4*>(this)->operator[](index);
	}


	Point::Point (coord value)
	{
		reset(value);
	}

	Point::Point (coord x, coord y, coord z)
	{
		reset(x, y, z);
	}

	Point
	Point::dup () const
	{
		return *this;
	}

	Point&
	Point::reset (coord value)
	{
		Super::reset(value);
		return *this;
	}

	Point&
	Point::reset (coord x, coord y, coord z)
	{
		Super::reset(x, y, z);
		return *this;
	}

	Point&
	Point::move_to (coord x, coord y, coord z)
	{
		reset(x, y, z);
		return *this;
	}

	Point&
	Point::move_to (const This& point)
	{
		reset(point.x, point.y, point.z);
		return *this;
	}

	Point&
	Point::move_by (coord x, coord y, coord z)
	{
		reset(this->x + x, this->y + y, this->z + z);
		return *this;
	}

	Point&
	Point::move_by (const This& point)
	{
		reset(this->x + point.x, this->y + point.y, this->z + point.z);
		return *this;
	}

	void
	Point::rotate (float degree)
	{
		to_glm(*this) = glm::rotateZ(to_glm(*this), Xot::deg2rad(degree));
	}

	coord
	Point::length () const
	{
		return glm::length(to_glm(*this));
	}

	Point
	Point::normal () const
	{
		if (dot(*this, *this) == 0)
			invalid_state_error(__FILE__, __LINE__);

		return to_rays(glm::normalize(to_glm(*this)));
	}

	void
	Point::normalize ()
	{
		*this = normal();
	}

	String
	Point::inspect () const
	{
		return Xot::stringf("x=%f y=%f z=%f", x, y, z);
	}

	Point
	Point::operator - () const
	{
		return to_rays(-to_glm(*this));
	}

	Point&
	Point::operator += (coord rhs)
	{
		to_glm(*this) += rhs;
		return *this;
	}

	Point&
	Point::operator += (const This& rhs)
	{
		to_glm(*this) += to_glm(rhs);
		return *this;
	}

	Point&
	Point::operator -= (coord rhs)
	{
		to_glm(*this) -= rhs;
		return *this;
	}

	Point&
	Point::operator -= (const This& rhs)
	{
		to_glm(*this) -= to_glm(rhs);
		return *this;
	}

	Point&
	Point::operator *= (coord rhs)
	{
		to_glm(*this) *= rhs;
		return *this;
	}

	Point&
	Point::operator *= (const This& rhs)
	{
		to_glm(*this) *= to_glm(rhs);
		return *this;
	}

	Point&
	Point::operator /= (coord rhs)
	{
		if (rhs == 0)
			argument_error(__FILE__, __LINE__);

		to_glm(*this) /= rhs;
		return *this;
	}

	Point&
	Point::operator /= (const This& rhs)
	{
		if (rhs.x == 0 || rhs.y == 0 || rhs.z == 0)
			argument_error(__FILE__, __LINE__);

		to_glm(*this) /= to_glm(rhs);
		return *this;
	}

	bool
	operator == (const Point& lhs, const Point& rhs)
	{
		return to_glm(lhs) == to_glm(rhs);
	}

	bool
	operator != (const Point& lhs, const Point& rhs)
	{
		return to_glm(lhs) != to_glm(rhs);
	}

	Point
	operator + (coord lhs, const Point& rhs)
	{
		return to_rays(lhs + to_glm(rhs));
	}

	Point
	operator + (const Point& lhs, coord rhs)
	{
		return to_rays(to_glm(lhs) + rhs);
	}

	Point
	operator + (const Point& lhs, const Point& rhs)
	{
		return to_rays(to_glm(lhs) + to_glm(rhs));
	}

	Point
	operator - (coord lhs, const Point& rhs)
	{
		return to_rays(lhs - to_glm(rhs));
	}

	Point
	operator - (const Point& lhs, coord rhs)
	{
		return to_rays(to_glm(lhs) - rhs);
	}

	Point
	operator - (const Point& lhs, const Point& rhs)
	{
		return to_rays(to_glm(lhs) - to_glm(rhs));
	}

	Point
	operator * (coord lhs, const Point& rhs)
	{
		return to_rays(lhs * to_glm(rhs));
	}

	Point
	operator * (const Point& lhs, coord rhs)
	{
		return to_rays(to_glm(lhs) * rhs);
	}

	Point
	operator * (const Point& lhs, const Point& rhs)
	{
		return to_rays(to_glm(lhs) * to_glm(rhs));
	}

	Point
	operator / (coord lhs, const Point& rhs)
	{
		if (rhs.x == 0 || rhs.y == 0 || rhs.z == 0)
			argument_error(__FILE__, __LINE__);

		return to_rays(lhs / to_glm(rhs));
	}

	Point
	operator / (const Point& lhs, coord rhs)
	{
		if (rhs == 0)
			argument_error(__FILE__, __LINE__);

		return to_rays(to_glm(lhs) / rhs);
	}

	Point
	operator / (const Point& lhs, const Point& rhs)
	{
		if (rhs.x == 0 || rhs.y == 0 || rhs.z == 0)
			argument_error(__FILE__, __LINE__);

		return to_rays(to_glm(lhs) / to_glm(rhs));
	}


	coord
	dot (const Point& p1, const Point& p2)
	{
		return glm::dot(to_glm(p1), to_glm(p2));
	}

	Point
	cross (const Point& p1, const Point& p2)
	{
		return to_rays(glm::cross(to_glm(p1), to_glm(p2)));
	}


}// Rays
