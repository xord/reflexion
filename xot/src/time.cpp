#include "xot/time.h"


#include <boost/date_time/posix_time/posix_time.hpp>

namespace pt = boost::posix_time;


namespace Xot
{


	double
	time (bool local)
	{
		static pt::ptime zero = pt::from_time_t(0);
		pt::ptime now = local ?
			pt::microsec_clock::local_time() :
			pt::microsec_clock::universal_time();
		return (now - zero).total_milliseconds() / 1000.0;
	}


}// Xot
