#include "reflex/selector.h"


#include <assert.h>
#include "reflex/exception.h"
#include "selector.h"


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

		const_iterator end = selector.end();
		for (const_iterator tag = selector.begin(); tag != end; ++tag)
		{
			if (tags.find(*tag) == tags_end)
				return false;
		}

		return selector.self->name.empty() || selector.self->name == self->name;
	}

	void
	Selector::set_name (const char* name)
	{
		if (name && *name != '\0')
			self->name = name;
		else
			self->name.clear();
	}

	const char*
	Selector::name () const
	{
		const String& s = self->name;
		return !s.empty() ? s.c_str() : NULL;
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

	bool
	Selector::has_tag (const char* tag) const
	{
		if (!tag || *tag == '\0')
			return false;

		return self->tags.find(tag) != self->tags.end();
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


	static Selector*
	get_selector (SelectorPtr* this_, bool create = false)
	{
		assert(this_);

		SelectorPtr& ptr = *this_;

		if (create && !ptr) ptr.reset(new Selector());
		return ptr.get();
	}

	static const Selector*
	get_selector (const SelectorPtr* this_)
	{
		return get_selector(const_cast<SelectorPtr*>(this_));
	}

	void
	SelectorPtr::set_name (const char* name)
	{
		get_selector(this, true)->set_name(name);
	}

	const char*
	SelectorPtr::name () const
	{
		const Selector* sel = get_selector(this);
		return sel ? sel->name() : NULL;
	}

	void
	SelectorPtr::add_tag (const char* tag)
	{
		get_selector(this, true)->add_tag(tag);
	}

	void
	SelectorPtr::remove_tag (const char* tag)
	{
		Selector* sel = get_selector(this);
		if (sel) sel->remove_tag(tag);
	}

	bool
	SelectorPtr::has_tag (const char* tag) const
	{
		const Selector* sel = get_selector(this);
		return sel ? sel->has_tag(tag) : false;
	}

	static Selector::TagSet empty_tags;

	Selector::iterator
	SelectorPtr::tag_begin ()
	{
		assert(empty_tags.empty());

		Selector* sel = get_selector(this);
		return sel ? sel->begin() : empty_tags.begin();
	}

	Selector::const_iterator
	SelectorPtr::tag_begin () const
	{
		return selector().begin();
	}

	Selector::iterator
	SelectorPtr::tag_end ()
	{
		assert(empty_tags.empty());

		Selector* sel = get_selector(this);
		return sel ? sel->end() : empty_tags.end();
	}

	Selector::const_iterator
	SelectorPtr::tag_end () const
	{
		return selector().end();
	}

	void
	SelectorPtr::set_selector (const Selector& selector)
	{
		*get_selector(this, true) = selector;
	}

	Selector&
	SelectorPtr::selector ()
	{
		return *get_selector(this, true);
	}

	const Selector&
	SelectorPtr::selector () const
	{
		static const Selector EMPTY;

		const Selector* sel = get_selector(this);
		return sel ? *sel : EMPTY;
	}


}// Reflex
