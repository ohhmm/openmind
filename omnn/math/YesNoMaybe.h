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

// Conversion to bool (explicit to prevent accidental conversions)
inline constexpr explicit operator bool(YesNoMaybe value) {
    return value == YesNoMaybe::Yes;
}

} // namespace math
} // namespace omnn
