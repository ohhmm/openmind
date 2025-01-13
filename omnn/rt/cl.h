#pragma once

#ifdef OPENMIND_USE_OPENCL
#include <boost/compute.hpp>
#include <boost/compute/core.hpp>

namespace omnn::rt {
// Returns the best available OpenCL device, or an empty device if:
// - SKIP_OPENCL_TESTS=1 environment variable is set
// - No OpenCL devices are available and we're in test mode
const ::boost::compute::device& GetComputeUnitsWinnerDevice();
}

#endif
