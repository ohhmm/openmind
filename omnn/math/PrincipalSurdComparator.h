#pragma once
#include <omnn/math/PrincipalSurd.h>

namespace omnn::math {

struct PrincipalSurdComparator {
    bool operator()(const PrincipalSurd& lhs, const PrincipalSurd& rhs) const {
        return lhs.get1() == rhs.get1() && lhs.get2() == rhs.get2();
    }
};

}
