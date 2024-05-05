#include "cl.h"

#ifdef OPENMIND_USE_OPENCL

#include <boost/compute.hpp>
#include <iostream>
#include <memory>

namespace {

std::unique_ptr<boost::compute::device> ComputeUnitsWinnerDevice;
std::unique_ptr<boost::compute::context> ComputeUnitsWinnerContext;

void InitializeComputeUnitsWinner() {
    try {
        auto devices = boost::compute::system::devices();
        if (devices.empty()) {
            std::cerr << "Please, install an OpenCL driver." << std::endl;
            exit(-1);
        } else {
            auto cuwinner = boost::compute::system::default_device();
            for (auto& p : boost::compute::system::platforms()) {
                for (auto& d : p.devices()) {
                    auto cu = d.compute_units();
                    if (cu > cuwinner.compute_units()) {
                        cuwinner = d;
                    }
                }
            }

            ComputeUnitsWinnerDevice = std::make_unique<boost::compute::device>(cuwinner);
            ComputeUnitsWinnerContext = std::make_unique<boost::compute::context>(*ComputeUnitsWinnerDevice);
        }
    } catch (...) {
        std::cerr << "Failed to initialize OpenCL device." << std::endl;
    }
}

} // namespace

namespace omnn::rt {

void InitializeOpenCL() {
    InitializeComputeUnitsWinner();
}

void ReleaseOpenCLResources() {
    ComputeUnitsWinnerContext.reset();
    ComputeUnitsWinnerDevice.reset();
}

const boost::compute::device& GetComputeUnitsWinnerDevice() {
    if (!ComputeUnitsWinnerDevice) {
        InitializeComputeUnitsWinner();
    }
    return *ComputeUnitsWinnerDevice;
}

const boost::compute::context& GetComputeUnitsWinnerContext() {
    if (!ComputeUnitsWinnerContext) {
        InitializeComputeUnitsWinner();
    }
    return *ComputeUnitsWinnerContext;
}

} // namespace omnn::rt

#endif
