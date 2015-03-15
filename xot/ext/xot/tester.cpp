#include <ruby.h>


#include "xot.h"


template <typename EXCEPTION>
static bool
test_exception (bool should_catch = true)
{
	try
	{
		throw EXCEPTION("");
	}
	catch (EXCEPTION& e)
	{
		if (!should_catch) return false;
	}
	catch (...)
	{
		if (should_catch) return false;
	}

	Xot::String s = "Exception Message";
	EXCEPTION e(s);
	if (e.what() != s) return false;

	return true;
}

static bool
test_exception ()
{
	if (!test_exception<Xot::XotError>())            return false;
	if (!test_exception<Xot::InvalidStateError>())   return false;
	if (!test_exception<Xot::SystemError>())         return false;
	if (!test_exception<Xot::NotImplementedError>()) return false;
	if (!test_exception<std::invalid_argument>())    return false;
	if (!test_exception<std::out_of_range>())        return false;
	return true;
}

struct RefObj : public Xot::RefCountable<> {};

static bool
test_ref ()
{
	if (!(Xot::Ref<RefObj>(NULL) == Xot::Ref<RefObj>(NULL)))       return false;
	if (!(Xot::Ref<RefObj>(NULL) != Xot::Ref<RefObj>(new RefObj))) return false;

	if (!(Xot::Ref<RefObj>(NULL) == Xot::Ref<const RefObj>(NULL)))       return false;
	if (!(Xot::Ref<RefObj>(NULL) != Xot::Ref<const RefObj>(new RefObj))) return false;

	if (!(Xot::Ref<const RefObj>(NULL) == Xot::Ref<RefObj>(NULL)))       return false;
	if (!(Xot::Ref<const RefObj>(NULL) != Xot::Ref<RefObj>(new RefObj))) return false;

	if (!(Xot::Ref<const RefObj>(NULL) == Xot::Ref<const RefObj>(NULL)))       return false;
	if (!(Xot::Ref<const RefObj>(NULL) != Xot::Ref<const RefObj>(new RefObj))) return false;

	if (!(Xot::Ref<RefObj>(NULL) == (RefObj*) NULL)) return false;
	if (!(Xot::Ref<RefObj>(NULL) != new RefObj))     return false;
	return true;
}

static bool
test_util ()
{
	if (!(Xot::clip(10, 100,  50) ==  50)) return false;
	if (!(Xot::clip(10, 100,  10) ==  10)) return false;
	if (!(Xot::clip(10, 100, 100) == 100)) return false;
	if (!(Xot::clip(10, 100,   0) ==  10)) return false;
	if (!(Xot::clip(10, 100, 200) == 100)) return false;

	int value;
	int* p = &value;
	if (!(Xot::get_pointer_flag(p) == false)) return false;

	p = Xot::set_pointer_flag(p);
	if (!(Xot::get_pointer_flag(p) == true))  return false;

	p = Xot::set_pointer_flag(p, false);
	if (!(Xot::get_pointer_flag(p) == false)) return false;

	p = Xot::set_pointer_flag(p, true);
	if (!(Xot::get_pointer_flag(p) == true))  return false;

	if (!(                      p         != &value))  return false;
	if (!(Xot::set_pointer_flag(p, false) == &value))  return false;

	return true;
}


static VALUE
test_native (VALUE self)
{
	if (!test_exception()) return false;
	if (!test_ref()) return false;
	if (!test_util()) return false;
	return true;
}


extern "C" void
Init_tester ()
{
	VALUE mXot    = rb_define_module(            "Xot");
	VALUE mTester = rb_define_module_under(mXot, "Tester");

	rb_define_singleton_method(
		mTester, "test_native", RUBY_METHOD_FUNC(test_native), 0);
}
