#include "architecture.h"

using namespace omnn::rt;
std::ostream& omnn::rt::operator<<(std::ostream& os, Architecture arch) {
    os << GetArchitectureName(arch);
    return os;
}
