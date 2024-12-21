#pragma once
#include <unordered_set>

namespace omnn::rt {

template <typename ValueT,
        typename ComparatorT = std::equal_to<ValueT>, 
        typename ContainerT = std::unordered_set<ValueT, std::hash<ValueT>, ComparatorT>
>
    class OptimizationLoopDetect {
    static thread_local ContainerT LoopDetectionStack;
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

template <typename ValueT, typename ComparatorT, typename ContainerT>
thread_local ContainerT OptimizationLoopDetect<ValueT, ComparatorT, ContainerT>::LoopDetectionStack;

}

#if !defined(NDEBUG) && !defined(NOOMDEBUG)
#define ANTILOOP(Type)                                                                                                 \
    ::omnn::rt::OptimizationLoopDetect<Type> antilooper(*this);                                                        \
    if (antilooper.isLoopDetected()) {                                                                                 \
        std::cout << "Loop of optimizing detected in " << *this << std::endl;                                          \
        return;                                                                                                        \
    }
#else
#define ANTILOOP(Type)                                                                                                 \
    ::omnn::rt::OptimizationLoopDetect<Type> antilooper(*this);                                                        \
    if (antilooper.isLoopDetected()) {                                                                                 \
        return;                                                                                                        \
    }
#endif
