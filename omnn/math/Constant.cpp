#include "Constant.h"

using namespace omnn::math;
using namespace constants;

std::map<std::string_view, Valuable> ConstNameAdder::SerializationNamesMap;

ConstNameAdder::ConstNameAdder(const std::string_view& name, const Valuable& obj) {
	SerializationNamesMap[name] = obj;
}
