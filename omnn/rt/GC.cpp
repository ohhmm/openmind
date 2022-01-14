#include "GC.h"

#include <cassert>
#include <chrono>

#include <boost/accumulators/statistics.hpp>
#include <boost/any.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/thread.hpp>

#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#include <sched.h>
#endif

namespace omnn::rt {



namespace {
boost::lockfree::stack<std::shared_ptr<void>> Bin(0);
auto ContinueIdleWork = true;
} // namespace
struct Deleter {
    ~Deleter() {
        ContinueIdleWork = {};
        GC::GCThread.join();
    }
};

std::thread GC::GCThread(GC::Routine);
Deleter GCThreadDeleter;


namespace {

void Nop(std::shared_ptr<void> obj) { }

} // namespace

void GC::DispatchDispose(std::shared_ptr<void>&& obj) {
    if (!GC::IsThreadGC() && ContinueIdleWork)
        Bin.push(obj);
}

bool GC::IsThreadGC() {
    return std::this_thread::get_id() == GCThread.get_id();
}

void GC::Routine() {
    auto h = GCThread.native_handle();
#ifdef _WIN32
    auto res = SetThreadPriority(h, THREAD_PRIORITY_IDLE);
    if (!res) {
        auto err = GetLastError();
        assert(res);
    }
#else
    sched_param sp={};
    sched_setscheduler(h, SCHED_IDLE, &sp);
#endif
    while (ContinueIdleWork) {
        if (Bin.consume_all(Nop)) {
            std::this_thread::yield();
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}


} // namespace omnn::rt
