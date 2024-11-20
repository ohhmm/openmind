#pragma once

namespace omnn {
namespace math {

// Represents a tri-state boolean value: Yes, No, or Maybe
enum class YesNoMaybe {
    No,
    Yes,
    Maybe,

    // Conversion to bool (explicit to prevent accidental conversions)
    constexpr explicit operator bool() const noexcept {
        return *this == YesNoMaybe::Yes;
    }
};

// Allow implicit conversion to bool for backward compatibility
// Returns true for Yes, false for No and Maybe
inline constexpr bool operator==(YesNoMaybe value, bool b) {
    return (value == YesNoMaybe::Yes) == b;
}

inline constexpr bool operator==(bool b, YesNoMaybe value) {
    return value == b;
}

inline constexpr bool operator!=(YesNoMaybe value, bool b) {
    return !(value == b);
}

inline constexpr bool operator!=(bool b, YesNoMaybe value) {
    return !(value == b);
}

} // namespace math
} // namespace omnn
