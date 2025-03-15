#pragma once
#include <unordered_set>

namespace omnn::rt {

template <typename ValueT,
        typename ComparatorT = std::equal_to<ValueT>, 
        typename ContainerT = std::unordered_set<ValueT, std::hash<ValueT>, ComparatorT>
>
class LoopDetectionGuard {
    static thread_local ContainerT LoopDetectionStack;
    bool isLoop;
    const ValueT* value;

public:
    LoopDetectionGuard(const ValueT& value) {
        auto emplaced = LoopDetectionStack.emplace(value);
        this->value = &*emplaced.first;
        isLoop = !emplaced.second;
    }

    ~LoopDetectionGuard() {
        if (!isLoop)
            LoopDetectionStack.erase(*value);
    }

    constexpr auto isLoopDetected() const { return isLoop; }
};

template <typename ValueT, typename ComparatorT, typename ContainerT>
thread_local ContainerT LoopDetectionGuard<ValueT, ComparatorT, ContainerT>::LoopDetectionStack;

}

#if !defined(NDEBUG) && !defined(NOOMDEBUG)
#define ANTILOOP(Type)                                                                                                 \
    ::omnn::rt::LoopDetectionGuard<Type> antilooper(*this);                                                            \
    if (antilooper.isLoopDetected()) {                                                                                 \
        std::cout << "LoopDetectionGuard detected a loop in " << __func__ << " : " << *this << std::endl;              \
        return;                                                                                                        \
    }
#else
#define ANTILOOP(Type)                                                                                                 \
    ::omnn::rt::LoopDetectionGuard<Type> antilooper(*this);                                                        \
    if (antilooper.isLoopDetected()) {                                                                                 \
        return;                                                                                                        \
    }
#endif
