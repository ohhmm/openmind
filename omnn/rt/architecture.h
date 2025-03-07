// architecture.h - Architecture detection utilities
#pragma once
#include <ostream>


namespace omnn::rt {

/**
 * Enumeration of supported CPU architectures
 */
enum class Architecture {
    Unknown,
    X86,
    X86_64,
    ARM,
    ARM64
};

/**
 * Get the current CPU architecture at compile time
 * @return The detected CPU architecture
 */
constexpr Architecture GetCurrentArchitecture() {
#if defined(__x86_64__) || defined(_M_X64)
    return Architecture::X86_64;
#elif defined(__i386__) || defined(_M_IX86)
    return Architecture::X86;
#elif defined(__aarch64__) || defined(_M_ARM64)
    return Architecture::ARM64;
#elif defined(__arm__) || defined(_M_ARM)
    return Architecture::ARM;
#else
    return Architecture::Unknown;
#endif
}

/**
 * Get the name of the architecture as a string
 * @param arch The architecture to get the name for
 * @return The architecture name as a string
 */
constexpr const char* GetArchitectureName(Architecture arch) {
    switch (arch) {
    case Architecture::X86: return "x86";
    case Architecture::X86_64: return "x86_64";
    case Architecture::ARM: return "ARM";
    case Architecture::ARM64: return "ARM64";
    default: return "Unknown";
    }
}

/**
 * Check if the current architecture is x86 (32-bit or 64-bit)
 * @return true if the architecture is x86 or x86_64, false otherwise
 */
constexpr bool IsX86Architecture() {
    const auto arch = GetCurrentArchitecture();
    return arch == Architecture::X86 || arch == Architecture::X86_64;
}

/**
 * Check if the current architecture is ARM (32-bit or 64-bit)
 * @return true if the architecture is ARM or ARM64, false otherwise
 */
constexpr bool IsArmArchitecture() {
    const auto arch = GetCurrentArchitecture();
    return arch == Architecture::ARM || arch == Architecture::ARM64;
}

// Stream operator for Architecture enum
std::ostream& operator<<(std::ostream& os, Architecture arch);

} // namespace omnn::rt
