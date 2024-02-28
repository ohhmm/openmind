#pragma once
#include <boost/multiprecision/cpp_int.hpp>

#ifndef NDEBUG
#include <functional>
#endif

namespace omnn::rt {
    using number_t = ::boost::multiprecision::cpp_int; 
	using prime_t = number_t;
	using prime_idx_t = size_t;
    const prime_t& prime(prime_idx_t);
    size_t primes();
    prime_idx_t greatest_prime_idx(const number_t& bound);
    const prime_t& get_greatest_prime(const number_t& bound);

#ifdef OPENMIND_PRIME_MINING
bool GrowPrime(const boost::multiprecision::cpp_int& upto, std::function<bool(boost::multiprecision::cpp_int)> is_prime);
void MineNextPrime();
#endif
}