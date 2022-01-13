#pragma once
#include <any>
#include <functional>
#include <memory>
#include <thread>


namespace omnn::rt {

class GC {
    static std::thread GCThread;
    static void Routine();
    static const int InitStatus;
    static int Init();
    static void DeInit();

public:
    GC();
    static bool IsThreadGC();
    static void DispatchDispose(void*);
    static void DispatchDispose(std::shared_ptr<void>);
    template<class T>
    static void DispatchDispose(std::shared_ptr<T>&& o) {
        DispatchDispose(std::reinterpret_pointer_cast<void>(o));
    }
    template <template <typename> typename Te, typename Ty>
    static void DispatchDispose(Te<Ty>&& obj) {
        DispatchDispose(obj.release());
    }
};


} // namespace omnn::rt
