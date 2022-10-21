#pragma once

#ifdef OPENMIND_USE_OPENCL
#include <boost/compute.hpp>
#else
namespace boost::compute {
class device;
}
#endif

namespace omnn::rt {
const boost::compute::device& GetComputeUnitsWinnerDevice();
}
