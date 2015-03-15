#include "reflex/window.h"


#include "reflex/view.h"
#include "reflex/exception.h"


namespace Reflex
{


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
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	Window::on_hide (Event* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	Window::on_close (Event* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	Window::on_update (UpdateEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	Window::on_draw (DrawEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	Window::on_move (FrameEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	Window::on_resize (FrameEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	Window::on_key_down (KeyEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	Window::on_key_up (KeyEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	Window::on_pointer_down (PointerEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	Window::on_pointer_up (PointerEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	void
	Window::on_pointer_move (PointerEvent* e)
	{
		if (!e)
			argument_error(__FILE__, __LINE__);
	}

	bool
	Window::operator ! () const
	{
		return !operator bool();
	}


}// Reflex
