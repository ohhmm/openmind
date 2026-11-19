#pragma once

#ifdef OPENMIND_USE_OPENCL
#include <boost/compute/core.hpp>

namespace omnn::rt {
const ::boost::compute::device& GetComputeUnitsWinnerDevice();
}

#endif
