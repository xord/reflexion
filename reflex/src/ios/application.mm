// -*- objc -*-
#include "reflex/application.h"


#include <assert.h>
#include <crt_externs.h>
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
		if (!initialized())
			reflex_error(__FILE__, __LINE__, "not initialized.");

		if (instance)
			reflex_error(__FILE__, __LINE__, "multiple application instances.");

		instance = this;
	}

	Application::~Application ()
	{
		instance = NULL;
	}

	void
	Application::start ()
	{
		UIApplicationMain(
			*_NSGetArgc(), *_NSGetArgv(), nil, NSStringFromClass([AppDelegate class]));
	}

	void
	Application::quit ()
	{
		if (!*this)
			invalid_state_error(__FILE__, __LINE__);

		not_implemented_error(__FILE__, __LINE__);
	}

	void
	Application::set_name (const char* name)
	{
		if (!name)
			argument_error(__FILE__, __LINE__);

		self->name = name;
	}

	const char*
	Application::name () const
	{
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
