// -*- c++ -*-
#pragma once
#ifndef __REFLEX_SRC_STYLE_H__
#define __REFLEX_SRC_STYLE_H__


#include <reflex/style.h>


namespace Reflex
{


	class View;


	bool Style_set_owner (Style* style, View* owner);

	bool Style_has_variable_lengths (const Style& style);

	void Style_clear_inherited_values (Style* style);

	void Style_override (Style* overridden, const Style& overrides);

	void Style_apply_to (const Style* style, View* view);

	void Style_get_default_flow (Style::Flow* main, Style::Flow* sub);


}// Reflex


#endif//EOH
