#include "reflex/style_length.h"


#include <math.h>
#include <boost/array.hpp>
#include "reflex/exception.h"


namespace Reflex
{


	struct StyleLength::Data
	{

		Value value;

		Unit unit;

		Data () : value(0), unit(NONE) {}

	};// StyleLength::Data


	StyleLength::StyleLength ()
	{
	}

	StyleLength::StyleLength (Value value, Unit unit)
	{
		reset(value, unit);
	}

	StyleLength::StyleLength (const char* str)
	{
		reset(str);
	}

	StyleLength
	StyleLength::copy () const
	{
		return StyleLength(value(), unit());
	}

	void
	StyleLength::reset (Value value, Unit unit)
	{
		if (unit != PIXEL) //unit < NONE || UNIT_LAST <= unit)
			argument_error(__FILE__, __LINE__);

		self->value = value;
		self->unit  = unit;
	}

	static StyleLength::Unit
	str2unit (const char* s)
	{
		     if (strcasecmp(s, "px") == 0) return StyleLength::PIXEL;
		else if (strcasecmp(s, "%")  == 0) return StyleLength::PERCENT;
		else                               return StyleLength::NONE;
	}

	static const char*
	unit2str (StyleLength::Unit unit)
	{
		switch (unit)
		{
			case StyleLength::PIXEL:   return "px";
			case StyleLength::PERCENT: return "%";
			default:                   return NULL;
		}
	}

	void
	StyleLength::reset (const char* str)
	{
		Value num;
		char suffix[256];
		int count = sscanf(str, "%f%s", &num, suffix);
		if (count != 2)
			argument_error(__FILE__, __LINE__);

		reset(num, str2unit(suffix));
	}

	StyleLength::Value
	StyleLength::value () const
	{
		return self->value;
	}

	StyleLength::Unit
	StyleLength::unit () const
	{
		return self->unit;
	}

	bool
	StyleLength::get_pixel (coord* pixel, coord parent_size) const
	{
		if (!pixel)
			argument_error(__FILE__, __LINE__);

		switch (self->unit)
		{
			case PIXEL:   *pixel = self->value; break;
			case PERCENT: *pixel = parent_size * self->value; break;
			default: return false;
		}

		return true;
	}

	String
	StyleLength::to_s () const
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		String num;
		if (fmod(self->value, 1) == 0)
			num = Xot::stringf("%d", (long) self->value);
		else
			num = Xot::stringf("%g", self->value);

		const char* suffix = unit2str(self->unit);;
		if (!suffix)
			invalid_state_error(__FILE__, __LINE__);

		return num + suffix;
	}

	StyleLength::operator bool () const
	{
		return NONE < self->unit && self->unit < UNIT_LAST;
	}

	bool
	StyleLength::operator ! () const
	{
		return !operator bool();
	}


	struct StyleLength2::Data
	{

	  boost::array<StyleLength, 2> array;

	};// StyleLength2::Data


	StyleLength2::StyleLength2 ()
	{
	}

	StyleLength2::StyleLength2 (const StyleLength& all)
	{
		StyleLength2& a = *this;
		a[0] = a[1] = all;
	}

	StyleLength2::StyleLength2 (const StyleLength& width, const StyleLength& height)
	{
		StyleLength2& a = *this;
		a[0] = width;
		a[1] = height;
	}

	StyleLength2
	StyleLength2::copy () const
	{
		return StyleLength2(width().copy(), height().copy());
	}

	void
	StyleLength2::set_width (const StyleLength& width)
	{
		(*this)[0] = width;
	}

	const StyleLength&
	StyleLength2::width () const
	{
		return (*this)[0];
	}

	void
	StyleLength2::set_height (const StyleLength& height)
	{
		(*this)[1] = height;
	}

	const StyleLength&
	StyleLength2::height () const
	{
		return (*this)[1];
	}

	size_t
	StyleLength2::size () const
	{
		return self->array.size();
	}

	StyleLength&
	StyleLength2::operator [] (size_t i)
	{
		return self->array[i];
	}

	const StyleLength&
	StyleLength2::operator [] (size_t i) const
	{
		return const_cast<StyleLength2*>(this)->operator[](i);
	}


	struct StyleLength4::Data
	{

	  boost::array<StyleLength, 4> array;

	};// StyleLength4::Data


	StyleLength4::StyleLength4 ()
	{
	}

	StyleLength4::StyleLength4 (const StyleLength& all)
	{
		StyleLength4& a = *this;
		a[0] = a[1] = a[2] = a[3] = all;
	}

	StyleLength4::StyleLength4 (const StyleLength& horizontal, const StyleLength& vertical)
	{
		StyleLength4& a = *this;
		a[0] = a[2] = horizontal;
		a[1] = a[3] = vertical;
	}

	StyleLength4::StyleLength4 (
		const StyleLength& left,
		const StyleLength& vertical,
		const StyleLength& right)
	{
		StyleLength4& a = *this;
		a[0] = left;
		a[2] = right;
		a[1] = a[3] = vertical;
	}

	StyleLength4::StyleLength4 (
		const StyleLength& left,
		const StyleLength& top,
		const StyleLength& right,
		const StyleLength& bottom)
	{
		StyleLength4& a = *this;
		a[0] = left;
		a[1] = top;
		a[2] = right;
		a[3] = bottom;
	}

	StyleLength4
	StyleLength4::copy () const
	{
		return StyleLength4(left().copy(), top().copy(), right().copy(), bottom().copy());
	}

	void
	StyleLength4::set_left (const StyleLength& left)
	{
		(*this)[0] = left;
	}

	const StyleLength&
	StyleLength4::left () const
	{
		return (*this)[0];
	}

	void
	StyleLength4::set_top (const StyleLength& top)
	{
		(*this)[1] = top;
	}

	const StyleLength&
	StyleLength4::top () const
	{
		return (*this)[1];
	}

	void
	StyleLength4::set_right (const StyleLength& right)
	{
		(*this)[2] = right;
	}

	const StyleLength&
	StyleLength4::right () const
	{
		return (*this)[2];
	}

	void
	StyleLength4::set_bottom (const StyleLength& bottom)
	{
		(*this)[3] = bottom;
	}

	const StyleLength&
	StyleLength4::bottom () const
	{
		return (*this)[3];
	}

	size_t
	StyleLength4::size () const
	{
		return self->array.size();
	}

	StyleLength&
	StyleLength4::operator [] (size_t i)
	{
		return self->array[i];
	}

	const StyleLength&
	StyleLength4::operator [] (size_t i) const
	{
		return const_cast<StyleLength4*>(this)->operator[](i);
	}


}// Reflex
