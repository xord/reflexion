#include "reflex/selector.h"


#include "reflex/exception.h"


namespace Reflex
{


	struct Selector::Data
	{

		String name;

		TagSet tags;

		bool operator == (const Data& rhs)
		{
			return name == rhs.name && tags == rhs.tags;
		}

	};// Selector::Data


	Selector::Selector (const char* name)
	{
		if (name) set_name(name);
	}

	void
	Selector::set_name (const char* name)
	{
		self->name = name ? name : "";
	}

	const char*
	Selector::name () const
	{
		return self->name.empty() ? NULL : self->name.c_str();
	}

	void
	Selector::add_tag (const char* tag)
	{
		if (!tag || *tag == '\0')
			argument_error(__FILE__, __LINE__);

		iterator it = self->tags.find(tag);
		if (it != self->tags.end()) return;

		self->tags.insert(tag);
	}

	void
	Selector::remove_tag (const char* tag)
	{
		if (!tag || *tag == '\0')
			argument_error(__FILE__, __LINE__);

		iterator it = self->tags.find(tag);
		if (it == self->tags.end()) return;

		self->tags.erase(it);
	}

	Selector::iterator
	Selector::begin ()
	{
		return self->tags.begin();
	}

	Selector::const_iterator
	Selector::begin () const
	{
		return self->tags.begin();
	}

	Selector::iterator
	Selector::end ()
	{
		return self->tags.end();
	}

	Selector::const_iterator
	Selector::end () const
	{
		return self->tags.end();
	}

	bool
	Selector::match (const This& obj) const
	{
		if (self->name.empty() && self->tags.empty())
			return false;

		const TagSet& obj_tags = obj.self->tags;
		const_iterator obj_end = obj_tags.end();

		iterator end = this->end();
		for (iterator it = begin(); it != end; ++it)
			if (obj_tags.find(*it) == obj_end)
				return false;

		return self->name.empty() || self->name == obj.self->name;
	}

	bool
	operator == (const Selector& lhs, const Selector& rhs)
	{
		return *lhs.self == *rhs.self;
	}

	bool
	operator != (const Selector& lhs, const Selector& rhs)
	{
		return !operator==(lhs, rhs);
	}


}// Reflex
