#pragma once
#include <omnn/rt/antiloop.hpp>
#include <omnn/math/Valuable.h>

namespace omnn::math {

using same_valuable_cmp_t = decltype([](const Valuable& val1, const Valuable& val2){return val1.Same(val2);});
using ValuableLoopDetect = LoopDetectionGuard<Valuable, same_valuable_cmp_t>;

#undef ANTILOOP
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
#define ANTILOOP(Type)                                                                                                 \
    ::omnn::math::ValuableLoopDetect antilooper(*this);                                                                \
    if (antilooper.isLoopDetected()) {                                                                                 \
        std::cout << "LoopDetectionGuard detected a loop in " << __func__ << " : " << *this << std::endl;              \
        return;                                                                                                        \
    }
#else
#define ANTILOOP(Type)                                                                                                 \
    ::omnn::math::ValuableLoopDetect antilooper(*this);                                                                \
    if (antilooper.isLoopDetected()) {                                                                                 \
        return;                                                                                                        \
    }
#endif
