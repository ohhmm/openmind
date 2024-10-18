#include "pi.h"
#include <numbers>

using namespace omnn::math;

constinit std::string_view Pi::SerializationName = "pi";

Pi::operator double() const { return std::numbers::pi; }
