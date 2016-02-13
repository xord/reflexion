#include "window.h"


#include "reflex/view.h"
#include "reflex/exception.h"


namespace Reflex
{


	static CreateRootViewFun create_root_view_fun = NULL;

	void
	Window_set_create_root_view_fun (CreateRootViewFun fun)
	{
		create_root_view_fun = fun;
	}

	View*
	Window_create_root_view ()
	{
		return create_root_view_fun ? create_root_view_fun() : new View();
	}


	const View*
	Window::root () const
	{
		return const_cast<Window*>(this)->root();
	}

	const View*
	Window::focus () const
	{
		return const_cast<Window*>(this)->focus();
	}

	const Painter*
	Window::painter () const
	{
		return const_cast<Window*>(this)->painter();
	}

	Point
	Window::from_screen (const Point& point) const
	{
		not_implemented_error(__FILE__, __LINE__);
		return 0;
	}

	Point
	Window::to_screen (const Point& point) const
	{
		not_implemented_error(__FILE__, __LINE__);
		return 0;
	}

	void
	Window::on_show (Event* e)
	{
	}

	void
	Window::on_hide (Event* e)
	{
	}

	void
	Window::on_close (Event* e)
	{
	}

	void
	Window::on_update (UpdateEvent* e)
	{
	}

	void
	Window::on_draw (DrawEvent* e)
	{
	}

	void
	Window::on_move (FrameEvent* e)
	{
	}

	void
	Window::on_resize (FrameEvent* e)
	{
	}

	void
	Window::on_key_down (KeyEvent* e)
	{
	}

	void
	Window::on_key_up (KeyEvent* e)
	{
	}

	void
	Window::on_pointer_down (PointerEvent* e)
	{
	}

	void
	Window::on_pointer_up (PointerEvent* e)
	{
	}

	void
	Window::on_pointer_move (PointerEvent* e)
	{
	}

	bool
	Window::operator ! () const
	{
		return !operator bool();
	}


}// Reflex
