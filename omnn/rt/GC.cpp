#ifdef OPENMIND_BUILD_GC
#include "GC.h"

#include "exit.h"

#ifdef OPENMIND_PRIME_MINING
#include "Prime.h"
#endif


#include <cassert>
#include <chrono>
#include <mutex>
#include <set>
#include <stack>

#include <boost/thread.hpp>

#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#include <sched.h>
#endif

namespace omnn::rt {

std::jthread GC::GCThread(GC::Routine); // TODO : multiple idle threads

namespace {
std::mutex bin_mutex;
std::stack<std::shared_ptr<void>> Bin;
void Nop(std::shared_ptr<void> obj) { }

} // namespace

void GC::DispatchDispose(std::shared_ptr<void>&& obj) {
    if (!GC::IsThreadGC()
        && !IsProcessExit()
        && !GC::GCThread.get_stop_token().stop_requested()
        )
        Bin.push(std::move(obj));
    }
}

bool GC::IsThreadGC() {
    return std::this_thread::get_id() == GCThread.get_id();
}

void GC::Routine() {
#ifdef _WIN32
    auto res = SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_IDLE);
    if (!res) {
        auto err = GetLastError();
        assert(res);
    }
#else
    // TODO: set thread priority bellow normal and elevate it on low memory event from OS
    //sched_param sp={};
    //sched_setscheduler(h, SCHED_IDLE, &sp);
#endif
    while (!GC::GCThread.get_stop_token().stop_requested()) {
        bool processed = false;
        {
            std::lock_guard<std::mutex> lock(bin_mutex);
            while (!Bin.empty()) {
                Nop(std::move(Bin.top()));
                Bin.pop();
                processed = true;
            }
        }

        if (processed) {
            std::this_thread::yield();
        } else {
#ifdef OPENMIND_PRIME_MINING
            // TODO : omnn::rt::MineNextPrime();  (see factorial)
            std::this_thread::yield();
#else
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
        }
    }
}


} // namespace omnn::rt
#endif
