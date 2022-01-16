#include "GC.h"

#include <cassert>
#include <chrono>
#include <set>

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

std::jthread GC::GCThread(GC::Routine); // TODO : multiple idle threads

namespace {
boost::lockfree::stack<std::shared_ptr<void>> Bin(0);
std::set<std::thread::id> GCThreadIDs; // TODO : multiple idle threads
void Nop(std::shared_ptr<void> obj) { }

} // namespace

void GC::DispatchDispose(std::shared_ptr<void>&& obj) {
    if (!GC::IsThreadGC() && !GC::GCThread.get_stop_token().stop_requested())
        Bin.push(obj);
}

bool GC::IsThreadGC() { return GCThreadIDs.find(std::this_thread::get_id()) != GCThreadIDs.end(); }

void GC::Routine() {
    GCThreadIDs.insert(std::this_thread::get_id());
#ifdef _WIN32
    auto res = SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_IDLE);
    if (!res) {
        auto err = GetLastError();
        assert(res);
    }
#else
    //sched_param sp={};
    //sched_setscheduler(h, SCHED_IDLE, &sp);
#endif
    while (!GC::GCThread.get_stop_token().stop_requested()) {
        if (Bin.consume_all(Nop)) {
            std::this_thread::yield();
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}


} // namespace omnn::rt
