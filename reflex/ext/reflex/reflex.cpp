#include <rucy.h>
#include "reflex/reflex.h"
#include "defs.h"


using namespace Rucy;


static
RUCY_DEF0(init)
{
	Reflex::init();
	return self;
}
RUCY_END

static
RUCY_DEF0(fin)
{
	Reflex::fin();
	return self;
}
RUCY_END


static Module mReflex;

void
Init_reflex ()
{
	mReflex = define_module("Reflex");
	mReflex.define_singleton_method("init!", init);
	mReflex.define_singleton_method("fin!", fin);
}


namespace Reflex
{


	Module
	reflex_module ()
	{
		return mReflex;
	}


}// Reflex
