#pragma once
#include <unordered_set>

namespace omnn::rt {

template <typename ValueT>
class OptimizationLoopDetect {
    static thread_local std::unordered_set<ValueT> LoopDetectionStack;
    bool isLoop;
    const ValueT* value;

public:
    OptimizationLoopDetect(const ValueT& value) {
        auto emplaced = LoopDetectionStack.emplace(value);
        this->value = &*emplaced.first;
        isLoop = !emplaced.second;
    }

    ~OptimizationLoopDetect() {
        if (!isLoop)
            LoopDetectionStack.erase(*value);
    }

    auto isLoopDetected() const { return isLoop; }
};

template <typename ValueT>
thread_local std::unordered_set<ValueT> OptimizationLoopDetect<ValueT>::LoopDetectionStack;

}

#if !defined(NDEBUG) && !defined(NOOMDEBUG)
#define ANTILOOP(Type)                                                                                                 \
    ::omnn::rt::OptimizationLoopDetect<Type> antilooper(*this);                                                        \
    if (antilooper.isLoopDetected()) {                                                                                 \
        std::cout << "Loop of optimizating detected in " << *this << std::endl;                                        \
        return;                                                                                                        \
    }
#else
#define ANTILOOP(Type)                                                                                                 \
    ::omnn::rt::OptimizationLoopDetect<Type> antilooper(*this);                                                        \
    if (antilooper.isLoopDetected()) {                                                                                 \
        return;                                                                                                        \
    }
#endif
