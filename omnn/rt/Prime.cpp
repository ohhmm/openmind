#include "Prime.h"
#include <cassert>
#include <thread>
#include <utility>

#define PRIME_LIST_FILENAME "Primes.inc"
static const boost::multiprecision::cpp_int Primes[] = {
#include PRIME_LIST_FILENAME 
};
static const size_t PrimeItems = sizeof(Primes) / sizeof(Primes[0]);


#ifndef NDEBUG
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <rt/tasq.h>

namespace omnn::rt {

namespace {
#ifdef SRC_DIR
boost::filesystem::path PrimeListDir = SRC_DIR;
auto PrimeListPath = PrimeListDir / PRIME_LIST_FILENAME;
#endif

bool GrowPrime(const boost::multiprecision::cpp_int& upto,
               std::function<bool(boost::multiprecision::cpp_int)> is_prime) {
    auto& prev = Primes[PrimeItems - 1];
    auto range = upto;
    range -= prev;
    auto chunks = std::thread::hardware_concurrency();
    auto chunk = range;
    range /= chunks;
    StoringTasksQueue primining;
    for (decltype(chunks) i = 0; i <= chunks; ++i) {
        primining.AddTask([=]() { 
            auto up = chunk;
            up *= i;
            for (decltype(up) j = 0; j < up; ++j) {
            
            }
            return false;
        });
    }
}
}
#endif


const boost::multiprecision::cpp_int& omnn::rt::prime(size_t idx) {
#ifndef NDEBUG
    auto haveThePrime = idx < PrimeItems;
    if (!haveThePrime) {
        std::cerr << "There is no " << idx
                  << "th prime in the table yet. Use GrowPrime call if you use debug version to increase the table for "
                     "next compile time. Currently the table has "
                  << PrimeItems << " elements" << std::endl;
        assert(haveThePrime);
    }
#endif
    return Primes[idx];
}

size_t omnn::rt::primes() { return PrimeItems - 1; }
} // namespace omnn::rt