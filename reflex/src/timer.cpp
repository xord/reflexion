#include "reflex/timer.h"


#include "reflex/view.h"
#include "reflex/event.h"
#include "reflex/exception.h"
#include "timer.h"
#include "selector.h"


namespace Reflex
{


	typedef Timer* (*CreateTimerFun) ();

	static CreateTimerFun create_timer_fun = NULL;

	void
	set_create_timer_fun (CreateTimerFun fun)
	{
		create_timer_fun = fun;
	}

	static Timer*
	create_timer ()
	{
		return create_timer_fun ? create_timer_fun() : new Timer();
	}


	struct Timer::Data
	{

		View* owner;

		int id, count;

		float interval;

		double next_time;

		SelectorPtr pselector;

		Data ()
		:	owner(NULL), id(ID_INVALID), count(1), interval(-1), next_time(-1)
		{
		}

		Selector& selector ()
		{
			if (!pselector) pselector.reset(new Selector);
			return *pselector;
		}

	};// Data


	Timer::Timer ()
	{
	}

	Timer::~Timer ()
	{
	}

	void
	Timer::fire ()
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		TimerEvent e(this);
		self->owner->on_timer(&e);
	}

	View*
	Timer::owner () const
	{
		return self->owner;
	}

	int
	Timer::id () const
	{
		return self->id;
	}

	float
	Timer::interval () const
	{
		return self->interval;
	}

	void
	Timer::set_count (int count)
	{
		self->count = count;
	}

	int
	Timer::count () const
	{
		return self->count;
	}

	bool
	Timer::is_finished () const
	{
		return self->count == 0;
	}

	void
	Timer::set_name (const char* name)
	{
		self->pselector.set_name(name);
	}

	const char*
	Timer::name () const
	{
		return self->pselector.name();
	}

	void
	Timer::add_tag (const char* tag)
	{
		self->pselector.add_tag(tag);
	}

	void
	Timer::remove_tag (const char* tag)
	{
		self->pselector.remove_tag(tag);
	}

	Timer::tag_iterator
	Timer::tag_begin ()
	{
		return self->pselector.tag_begin();
	}

	Timer::const_tag_iterator
	Timer::tag_begin () const
	{
		return self->pselector.tag_begin();
	}

	Timer::tag_iterator
	Timer::tag_end ()
	{
		return self->pselector.tag_end();
	}

	Timer::const_tag_iterator
	Timer::tag_end () const
	{
		return self->pselector.tag_end();
	}

	void
	Timer::set_selector (const Selector& selector)
	{
		self->pselector.set_selector(selector);
	}

	Selector&
	Timer::selector ()
	{
		return self->pselector.selector();
	}

	const Selector&
	Timer::selector () const
	{
		return self->pselector.selector();
	}

	Timer::operator bool () const
	{
		const Data* p = self.get();
		return p->owner && p->id != ID_INVALID && p->interval >= 0;
	}

	bool
	Timer::operator ! () const
	{
		return !operator bool();
	}

	bool
	operator == (const Timer& lhs, const Timer& rhs)
	{
		return lhs.self.get() == rhs.self.get();
	}

	bool
	operator != (const Timer& lhs, const Timer& rhs)
	{
		return !operator==(lhs, rhs);
	}


	Timers::Timers ()
	:	next_id(Timer::ID_FIRST)
	{
	}

	void
	Timers::add (Timer* timer)
	{
		assert(timer && *timer);

		List::iterator end_ = timers.end();
		for (List::iterator it = timers.begin(); it != end_; ++it)
		{
			if (timer->self->next_time <= (*it)->self->next_time)
				continue;

			timers.insert(it, timer);
			return;
		}

		timers.push_back(timer);
	}

	static void
	update_next_time (Timer* timer)
	{
		assert(timer && *timer);

		Timer::Data* self = timer->self.get();

		self->next_time = Xot::time() + self->interval;
	}

	static void
	setup_timer (Timer* timer, View* owner, int id, float interval, int count)
	{
		assert(timer);

		Timer::Data* self = timer->self.get();

		self->owner    = owner;
		self->id       = id;
		self->interval = interval;
		self->count    = count;

		update_next_time(timer);
	}

	Timer*
	Timers::add (View* owner, float interval, int count)
	{
		if (!owner)
			argument_error(__FILE__, __LINE__);

		Timer* timer = create_timer();
		if (!timer)
			reflex_error(__FILE__, __LINE__, "failed to create timer.");

		setup_timer(timer, owner, next_id++, interval, count);
		add(timer);
		return timer;
	}

	void
	Timers::remove (Timer* timer)
	{
		if (!timer)
			argument_error(__FILE__, __LINE__);

		List::iterator end_ = timers.end();
		for (List::iterator it = timers.begin(); it != end_; ++it)
		{
			if (timer->id() == (*it)->id())
				timers.erase(it);
		}
	}

	static bool
	is_time_to_fire (Timer* timer, double now)
	{
		assert(timer);

		return now >= timer->self->next_time;
	}

	static void
	fire_timer (Timer* timer)
	{
		assert(timer && !timer->is_finished());

		if (timer->self->count > 0)
			--timer->self->count;

		timer->fire();
	}

	void
	Timers::fire (double now)
	{
		List continues;

		List::iterator timers_end = timers.end();
		for (List::iterator it = timers.begin(); it != timers_end;)
		{
			Timer* timer = it->get();
			if (!is_time_to_fire(timer, now))
			{
				++it;
				continue;
			}

			fire_timer(timer);
			if (!timer->is_finished())
			{
				update_next_time(timer);
				continues.push_back(timer);
			}

			it = timers.erase(it);
		}

		List::iterator continues_end = continues.end();
		for (List::iterator it = continues.begin(); it != continues_end; ++it)
			add(it->get());
	}

	void
	Timers::find_timers (
		TimerList* result, const Selector& selector, bool recursive) const
	{
		if (!result)
			argument_error(__FILE__, __LINE__);

		not_implemented_error(__FILE__, __LINE__);
	}


}// Reflex
