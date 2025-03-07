#pragma once
#include <ostream>

namespace omnn::rt {

// Architecture enumeration for different compute architectures
enum class Architecture {
    CPU,
    GPU,
    FPGA,
    TPU,
    NPU,
    DSP,
    ASIC,
    Unknown
};

// Stream operator for Architecture enum
inline std::ostream& operator<<(std::ostream& os, const Architecture& arch) {
    switch (arch) {
        case Architecture::CPU:
            os << "CPU";
            break;
        case Architecture::GPU:
            os << "GPU";
            break;
        case Architecture::FPGA:
            os << "FPGA";
            break;
        case Architecture::TPU:
            os << "TPU";
            break;
        case Architecture::NPU:
            os << "NPU";
            break;
        case Architecture::DSP:
            os << "DSP";
            break;
        case Architecture::ASIC:
            os << "ASIC";
            break;
        case Architecture::Unknown:
        default:
            os << "Unknown";
            break;
    }
    return os;
}

} // namespace omnn::rt
