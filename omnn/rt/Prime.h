#pragma once
#include <boost/multiprecision/cpp_int.hpp>

#ifndef NDEBUG
#include <functional>
#endif

namespace omnn::rt {
const boost::multiprecision::cpp_int
#ifndef OPENMIND_PRIME_TABLE_BOOST
        &
#endif
        prime(size_t idx);
size_t primes();

#ifdef OPENMIND_PRIME_MINING
bool GrowPrime(const boost::multiprecision::cpp_int& upto, std::function<bool(boost::multiprecision::cpp_int)> is_prime);
#endif
}