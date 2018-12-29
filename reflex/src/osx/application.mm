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


	namespace global
	{

		static Application* instance = NULL;

	}// global


	Application*
	app ()
	{
		return global::instance;
	}


	Application::Application ()
	{
		if (!initialized() || !NSApp)
			reflex_error(__FILE__, __LINE__, "not initialized.");

		if (global::instance)
			reflex_error(__FILE__, __LINE__, "multiple application instances.");

		ReflexAppDelegate* delegate = (ReflexAppDelegate*) [NSApp delegate];
		if (!delegate)
			invalid_state_error(__FILE__, __LINE__);

		[delegate bind: this];

		global::instance = this;
	}

	Application::~Application ()
	{
		global::instance = NULL;
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
