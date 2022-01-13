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
boost::lockfree::stack<void*> Bin;
boost::lockfree::stack<std::shared_ptr<void>> ShBin;
auto ContinueIdleWork = true;
} // namespace

const int GC::InitStatus = GC::Init();
std::thread GC::GCThread(GC::Routine);

int GC::Init() {
    return atexit(GC::DeInit);
}

void GC::DeInit() {
    GC::GCThread.join();
    ContinueIdleWork = {};
}


namespace {

void Call(const std::function<void()>& f) {
    f();
}

void Del(void* obj) { delete obj; }

void Nop(std::shared_ptr<void> obj) { }

} // namespace

void GC::DispatchDispose(void* obj) {
    if (GC::IsThreadGC()) {
        Del(obj);
    } else {
        Bin.push(obj);
    }
}

void GC::DispatchDispose(std::shared_ptr<void> obj) { 
    if (!GC::IsThreadGC()) 
        ShBin.push(obj);
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
    sched_setscheduler(h, SHED_IDLE, &sp);
#endif
    while (ContinueIdleWork) {

        if ((Bin.empty() ? 0 : Bin.consume_all(Del)) + (ShBin.empty() ? 0 : ShBin.consume_all(Nop))) {
            std::this_thread::yield();
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

GC::GC() {}

} // namespace omnn::rt
