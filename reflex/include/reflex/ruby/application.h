// -*- c++ -*-
#pragma once
#ifndef __REFLEX_RUBY_APPLICATION_H__
#define __REFLEX_RUBY_APPLICATION_H__


#include <rucy/rucy.h>
#include <rucy/class.h>
#include <rucy/extension.h>
#include <reflex/application.h>
#include <reflex/ruby/event.h>


namespace Reflex
{


	Rucy::Class application_class ();
	// class Reflex::Application


	template <typename T>
	class RubyApplication : public Rucy::ClassWrapper<T>
	{

		public:

			RUCY_OVERRIDE_BEGIN(Rucy::ClassWrapper<T>)
			RUCY_OVERRIDE_END

			virtual void start ()
			{
				RUCY_SYM(start);
				if (RUCY_IS_OVERRIDABLE())
					this->value.call(start);
				else
					return Super::start();
			}

			virtual void quit ()
			{
				RUCY_SYM(quit);
				if (RUCY_IS_OVERRIDABLE())
					this->value.call(quit);
				else
					return Super::quit();
			}

			virtual void on_start (Event* e)
			{
				RUCY_SYM(on_start);
				if (RUCY_IS_OVERRIDABLE())
					this->value.call(on_start, Rucy::value(e));
				else
					return Super::on_start(e);
			}

			virtual void on_quit (Event* e)
			{
				RUCY_SYM(on_quit);
				if (RUCY_IS_OVERRIDABLE())
					this->value.call(on_quit, Rucy::value(e));
				else
					return Super::on_quit(e);
			}

			virtual void on_preference (Event* e)
			{
				RUCY_SYM(on_preference);
				if (RUCY_IS_OVERRIDABLE())
					this->value.call(on_preference, Rucy::value(e));
				else
					return Super::on_preference(e);
			}

			virtual void on_about (Event* e)
			{
				RUCY_SYM(on_about);
				if (RUCY_IS_OVERRIDABLE())
					this->value.call(on_about, Rucy::value(e));
				else
					return Super::on_about(e);
			}

	};// RubyApplication


}// Reflex


RUCY_DECLARE_WRAPPER_VALUE_FROM_TO(Reflex::Application)


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Reflex::Application> ()
	{
		return Reflex::application_class();
	}

	inline Value
	value (Reflex::Application::Ref& ref, Value klass = Reflex::application_class())
	{
		return value(ref.get(), klass);
	}


}// Rucy


#endif//EOH
