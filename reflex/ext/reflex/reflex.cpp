#include <rucy.h>
#include "reflex/reflex.h"
#include "reflex/ruby/view.h"
#include "defs.h"


using namespace Rucy;


namespace Reflex
{
	typedef View* (*CreateRootViewFun) ();
	void set_create_root_view_fun (CreateRootViewFun);
}

static Reflex::View*
create_root_view ()
{
	return new Reflex::RubyView<Reflex::View>();
}


static
RUCY_DEF0(init)
{
	Reflex::init();
	Reflex::set_create_root_view_fun(create_root_view);

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
