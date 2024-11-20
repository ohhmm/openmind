#pragma once

namespace omnn {
namespace math {

// Represents a tri-state boolean value: Yes, No, or Maybe
enum class YesNoMaybe {
    No,
    Yes,
    Maybe
};

// Allow implicit conversion to bool for backward compatibility
// Returns true for Yes, false for No and Maybe
inline constexpr bool operator==(YesNoMaybe value, bool b) noexcept {
    return (value == YesNoMaybe::Yes) == b;
}

inline constexpr bool operator==(bool b, YesNoMaybe value) noexcept {
    return value == b;
}

inline constexpr bool operator!=(YesNoMaybe value, bool b) noexcept {
    return !(value == b);
}

inline constexpr bool operator!=(bool b, YesNoMaybe value) noexcept {
    return !(value == b);
}

// YesNoMaybe comparison operators
inline constexpr bool operator==(YesNoMaybe lhs, YesNoMaybe rhs) noexcept {
    return static_cast<int>(lhs) == static_cast<int>(rhs);
}

inline constexpr bool operator!=(YesNoMaybe lhs, YesNoMaybe rhs) noexcept {
    return !(lhs == rhs);
}

} // namespace math
} // namespace omnn
