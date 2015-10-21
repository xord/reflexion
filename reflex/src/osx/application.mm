// -*- objc -*-
#include "reflex/application.h"


#include <assert.h>
#import <AppKit/NSApplication.h>
#include "reflex/exception.h"
#include "application_data.h"
#import "app_delegate.h"


namespace Reflex
{


	bool initialized ();


	static Application* instance = NULL;


	Application*
	app ()
	{
		return instance;
	}


	Application::Application ()
	{
		if (!initialized() || !NSApp)
			reflex_error(__FILE__, __LINE__, "not initialized.");

		if (instance)
			reflex_error(__FILE__, __LINE__, "multiple application instances.");

		AppDelegate* delegate = (AppDelegate*) [NSApp delegate];
		if (!delegate)
			invalid_state_error(__FILE__, __LINE__);

		[delegate bind: this];

		instance = this;
	}

	Application::~Application ()
	{
		instance = NULL;
	}

	void
	Application::start ()
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		[NSApp run];
	}

	void
	Application::quit ()
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		[NSApp terminate: nil];
	}

	void
	Application::set_name (const char* name)
	{
		if (!name)
			argument_error(__FILE__, __LINE__);

		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		self->name = name;
	}

	const char*
	Application::name () const
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		return self->name.c_str();
	}

	void
	Application::on_start (Event* e)
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);
	}

	void
	Application::on_quit (Event* e)
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);
	}

	void
	Application::on_motion (MotionEvent* e)
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);
	}

	void
	Application::on_preference (Event* e)
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);
	}

	void
	Application::on_about (Event* e)
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		[NSApp orderFrontStandardAboutPanel: nil];
	}

	Application::operator bool () const
	{
		return self && *self;
	}

	bool
	Application::operator ! () const
	{
		return !operator bool();
	}


}// Reflex
