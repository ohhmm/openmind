#include "NaN.h"

namespace omnn::math {

constinit std::string_view NaN::SerializationName = "NaN";

NaN::NaN() {
	//std::cout << "nan" << std::endl;
}

NaN::NaN(encapsulated_instance&& why)
	: reason(std::move(why))
{}

} // namespace omnn::math