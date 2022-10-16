#pragma once

#include <boost/compute.hpp>

namespace omnn::rt {
const boost::compute::device& GetComputeUnitsWinnerDevice();
}
