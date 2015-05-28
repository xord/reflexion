#include "reflex/selector.h"


#include "reflex/exception.h"


namespace Reflex
{


	struct Selector::Data
	{

		String name;

		TagSet tags;

		friend bool operator == (const Data& lhs, const Data& rhs)
		{
			return lhs.name == rhs.name && lhs.tags == rhs.tags;
		}

	};// Selector::Data


	Selector::Selector (const char* name)
	{
		if (name) set_name(name);
	}

	Selector
	Selector::copy () const
	{
		Selector t;
		*t.self = *self;
		return t;
	}

	bool
	Selector::contains (const This& selector) const
	{
		if (self->name.empty() && self->tags.empty())
			return false;

		const TagSet& tags      = self->tags;
		const_iterator tags_end = tags.end();

		iterator end = selector.end();
		for (iterator tag = selector.begin(); tag != end; ++tag)
			if (tags.find(*tag) == tags_end)
				return false;

		return selector.self->name.empty() || selector.self->name == self->name;
	}

	void
	Selector::set_name (const char* name)
	{
		self->name = name ? name : "";
	}

	const char*
	Selector::name () const
	{
		return self->name.c_str();
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
	Selector::is_empty () const
	{
		return self->name.empty() && self->tags.empty();
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

	bool
	operator < (const Selector& lhs, const Selector& rhs)
	{
		Selector::Data* l = lhs.self.get();
		Selector::Data* r = rhs.self.get();
		return l->name < r->name || l->tags < r->tags;
	}


}// Reflex
