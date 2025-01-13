#include "cl.h"

#ifdef OPENMIND_USE_OPENCL

#include <boost/compute.hpp>

namespace {

auto ComputeUnitsWinner = []() -> boost::compute::device {
    try {
        // Skip OpenCL initialization if tests are running
        const char* skip_tests = std::getenv("SKIP_OPENCL_TESTS");
        if (skip_tests && *skip_tests == '1') {
            return {};
        }

        auto devices = boost::compute::system::devices();
        if (devices.size() == 0) {
            std::cout << "Please, install an OpenCL driver." << std::endl;
            if (!skip_tests) {
                exit(-1);
            }
            return {};
        } else if (devices.size() == 1) {
            auto& device = *devices.begin();
            std::cout << device.name() << " max_work_group_size=" << device.max_work_group_size() << std::endl;
            return device;
        } else {
            auto cuwinner = boost::compute::system::default_device();
            for (auto& p : boost::compute::system::platforms()) {
                for (auto& d : p.devices()) {
                    auto cu = d.compute_units();
                    auto widm = d.max_work_item_dimensions();
                    std::cout << std::endl << d.name() << ':' << std::endl;
                    std::cout << "\tversion: " << d.version() << std::endl;
                    std::cout << "\tcompute_units: " << cu << std::endl;
                    std::cout << "\tmax_work_item_dimensions: " << widm << std::endl;
                    std::cout << "\taddress_bits: " << d.address_bits() << std::endl;
                    std::cout << "\tclock_frequency: " << d.clock_frequency() << std::endl;
                    std::cout << "\tmax_work_group_size: " << d.max_work_group_size() << std::endl;
                    std::cout << "\textensions:";
                    bool glSharing = false;
                    for (auto& s : d.extensions()) {
                        std::cout << ' ' << s;
#ifdef OPENMIND_USE_OPENGL
                        if (s == "cl_khr_gl_sharing") {
                            std::cout << " (OpenGL sharing)";
                            glSharing = true;
                        }
#endif // OPENMIND_USE_OPENGL
                    }
                    std::cout << std::endl;
                    //<< cu << "cu x " << widm << " = " << cu * widm
                    if (cu > cuwinner.compute_units()
#ifdef OPENMIND_USE_OPENGL
                        && glSharing
#endif
                    )
                        cuwinner = d;
                }
            }

            std::cout << std::endl << "Selected " << cuwinner.name() << std::endl;
            std::cout << "\tmax_work_group_size: " << cuwinner.max_work_group_size() << std::endl << std::endl;
            return cuwinner;
        }
    } catch (...) {
        return {};
    }
}();

} // namespace


namespace omnn::rt {
const boost::compute::device& GetComputeUnitsWinnerDevice() { return ComputeUnitsWinner; }
}

#endif
