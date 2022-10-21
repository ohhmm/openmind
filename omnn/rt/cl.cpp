#include "cl.h"

#ifdef OPENMIND_USE_OPENCL

#include <boost/archive/binary_oarchive.hpp>
#include <boost/compute.hpp>
#include <boost/compute/core.hpp>
#include <boost/lambda2.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/program_options.hpp>
#include <boost/serialization/vector.hpp>

namespace {

auto ComputeUnitsWinner = []() -> boost::compute::device {
    auto devices = boost::compute::system::devices();
    if (devices.size() == 0) {
        std::cout << "Please, install an OpenCL driver." << std::endl;
        exit(-1);
    }

    auto cuwinner = boost::compute::system::default_device();
    for (auto& p : boost::compute::system::platforms()) {
        for (auto& d : p.devices()) {
            std::cout << d.name() << ':' << d.compute_units() << std::endl;
            if (d.compute_units() > cuwinner.compute_units())
                cuwinner = d;
        }
    }

    std::cout << "max_work_group_size: " << cuwinner.max_work_group_size() << std::endl;

    return cuwinner;
}();

} // namespace


namespace omnn::rt {
const boost::compute::device& GetComputeUnitsWinnerDevice() { return ComputeUnitsWinner; }
}

#endif