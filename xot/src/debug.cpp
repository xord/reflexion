#include "xot/debug.h"


#include <stdio.h>
#include <stdarg.h>
#include "xot/string.h"

// compile to check
#include "xot/pimpl.h"
#include "xot/ref.h"


namespace Xot
{


#ifdef XOT_USE_DOUT

	void
	dout (const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		printf("%s", stringf(format, args).c_str());
		va_end(args);
	}

	void
	doutln (const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		printf("%s\n", stringf(format, args).c_str());
		va_end(args);
	}

#endif


}// Xot
