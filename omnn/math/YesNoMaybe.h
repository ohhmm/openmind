#pragma once
#include <cstdint>

namespace omnn {
namespace math {

// Represents a tri-state boolean value: Yes, No, or Maybe
enum class YesNoMaybe : uint8_t {
    Yes = 0b1,
    Maybe = 0b10,
    No = 0b100
};


// constexpr operator bool(YesNoMaybe _) { return _==YesNoMaybe::Yes; }
constexpr bool operator!(YesNoMaybe _) noexcept {
    return _ == YesNoMaybe::No;
}

[[nodiscard]]
constexpr YesNoMaybe operator~(YesNoMaybe _) noexcept {
    constexpr YesNoMaybe InverseMap[] = {
        // Yes = 1, Maybe = 10, No = 100
        YesNoMaybe::Maybe, // 000 ?
        YesNoMaybe::No,    // 001 yes
        YesNoMaybe::Maybe, // 010 maybe
        YesNoMaybe::Maybe, // 011 ?
        YesNoMaybe::Yes,   // 100 no
        YesNoMaybe::Maybe, // 101 ?
        YesNoMaybe::Maybe, // 110 ?
        YesNoMaybe::Maybe, // 111 ?
    };
    return InverseMap[static_cast<uint8_t>(_)];
}

constexpr YesNoMaybe operator||(YesNoMaybe _1, YesNoMaybe _2) noexcept {
    constexpr YesNoMaybe OrMap[] = {
        // Yes = 1, Maybe = 10, No = 100
        YesNoMaybe::Maybe, // 000 bug
        YesNoMaybe::Yes,   // 001 yes
        YesNoMaybe::Maybe, // 010 maybe
        YesNoMaybe::Yes,   // 011 yes, maybe
        YesNoMaybe::No,    // 100 no
        YesNoMaybe::Yes,   // 101 yes,no
        YesNoMaybe::Maybe, // 110 maybe,no
        YesNoMaybe::Yes,   // 111 yes,maybe,no
    };
    return OrMap[static_cast<uint8_t>(_1) | static_cast<uint8_t>(_2)];
}

constexpr YesNoMaybe operator&&(YesNoMaybe _1, YesNoMaybe _2) noexcept {
    constexpr YesNoMaybe AndMap[] = {
        // Yes = 1, Maybe = 10, No = 100
        YesNoMaybe::Maybe, // 000 bug
        YesNoMaybe::Yes,   // 001 yes
        YesNoMaybe::Maybe, // 010 maybe
        YesNoMaybe::Maybe, // 011 yes, maybe
        YesNoMaybe::No,    // 100 no
        YesNoMaybe::No,    // 101 yes,no
        YesNoMaybe::No,    // 110 maybe,no
        YesNoMaybe::No,    // 111 yes,maybe,no
    };
    return AndMap[static_cast<uint8_t>(_1) | static_cast<uint8_t>(_2)];
}

// DONT: Allow implicit conversion to bool for backward compatibility
// Returns true for Yes, false for No and Maybe
//constexpr bool operator==(YesNoMaybe value, bool b) noexcept {
//    return (value == YesNoMaybe::Yes) == b;
//}
//
//constexpr bool operator==(bool b, YesNoMaybe value) noexcept {
//    return value == b;
//}
//
//constexpr bool operator!=(YesNoMaybe value, bool b) noexcept {
//    return !(value == b);
//}
//
//constexpr bool operator!=(bool b, YesNoMaybe value) noexcept {
//    return !(value == b);
//}

//// YesNoMaybe comparison operators
//constexpr bool operator==(YesNoMaybe lhs, YesNoMaybe rhs) noexcept {
//    return static_cast<int>(lhs) == static_cast<int>(rhs);
//}
//
//constexpr bool operator!=(YesNoMaybe lhs, YesNoMaybe rhs) noexcept {
//    return !(lhs == rhs);
//}

} // namespace math
} // namespace omnn
