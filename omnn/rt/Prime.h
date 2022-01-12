#pragma once
#include <boost/multiprecision/cpp_int.hpp>

#ifndef NDEBUG
#include <functional>
#endif

namespace omnn::rt {
const boost::multiprecision::cpp_int& prime(size_t idx);
size_t primes();

#ifndef NDEBUG
bool GrowPrime(const boost::multiprecision::cpp_int& upto, std::function<bool(boost::multiprecision::cpp_int)> is_prime);
#endif
}