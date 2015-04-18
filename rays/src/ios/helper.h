// -*- c++ -*-
#pragma once
#ifndef __RAYS_SRC_OSX_HELPER_H__
#define __RAYS_SRC_OSX_HELPER_H__


#include <boost/shared_ptr.hpp>
#include <CoreFoundation/CoreFoundation.h>


namespace Rays
{


	void safe_cfrelease (CFTypeRef ref);


	typedef boost::shared_ptr<const __CFString> CFString;

	CFString cfstring (const char* str);


}// Rays


#endif//EOH
