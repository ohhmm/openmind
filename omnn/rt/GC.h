#pragma once
#ifdef OPENMIND_BUILD_GC
#include <any>
#include <functional>
#include <memory>
#include <thread>


namespace omnn::rt {

class GC {
    static std::jthread GCThread;
    static void Routine();

public:
    static bool IsThreadGC();
    static void DispatchDispose(std::shared_ptr<void>&&);
    template<class T>
    static void DispatchDispose(std::shared_ptr<T>&& o) {
        DispatchDispose(std::move(std::reinterpret_pointer_cast<void>(std::move(o))));
    }
    template <template <typename> typename Te, typename Ty>
    static void DispatchDispose(Te<Ty>&& obj) {
        DispatchDispose(obj.release());
    }
};


} // namespace omnn::rt
#endif
