#include "rucy.h"


using namespace Rucy;


static
RUCY_DEFN(true_to_value)
{
	return value(true);
}
RUCY_END

static
RUCY_DEFN(false_to_value)
{
	return value(false);
}
RUCY_END

static
RUCY_DEFN(NULL_to_value)
{
	return nil();
}
RUCY_END

static
RUCY_DEFN(nil_value)
{
	return nil();
}
RUCY_END

static
RUCY_DEFN(array_value)
{
	const Value* a = argv;
	switch (argc)
	{
		case 1:  return array(a[0]);
		case 2:  return array(a[0], a[1]);
		case 3:  return array(a[0], a[1], a[2]);
		case 4:  return array(a[0], a[1], a[2], a[3]);
		case 5:  return array(a[0], a[1], a[2], a[3], a[4]);
		case 6:  return array(a[0], a[1], a[2], a[3], a[4], a[5]);
		case 7:  return array(a[0], a[1], a[2], a[3], a[4], a[5], a[6]);
		case 8:  return array(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
		case 9:  return array(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]);
		case 10: return array(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9]);
		case 0:
		default: argument_error(__FILE__, __LINE__);
	}
}
RUCY_END


void
Init_value ()
{
	Module mRucy   =       define_module("Rucy");
	Module mTester = mRucy.define_module("Tester");

	mTester.define_method("true_to_value", true_to_value);
	mTester.define_method("false_to_value", false_to_value);
	mTester.define_method("null_to_value", NULL_to_value);
	mTester.define_method("nil_value", nil_value);
	mTester.define_method("array_value", array_value);
}
