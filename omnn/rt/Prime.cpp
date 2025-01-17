#include "Prime.h"

#include <algorithm>
#include <cassert>
#include <thread>
#include <utility>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/math/special_functions/factorials.hpp>
#include <boost/math/special_functions/prime.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/numeric/conversion/cast.hpp>
#ifndef OPENMIND_PRIME_TABLE_OM
#include <boost/tokenizer.hpp>
#endif

#include <omnn/rt/lookup.hpp>
#include <omnn/rt/tasq.h>


#define PRIME_LIST_FILENAME "Primes.inc"

static omnn::rt::PrimeLookupTable Primes =
#ifdef OPENMIND_PRIME_TABLE_OM
{
#include PRIME_LIST_FILENAME
};
#else
[]() {
    constexpr auto size = boost::math::max_prime + 1;
    std::array<boost::multiprecision::cpp_int, size> numbers;
    for (uint32_t idx = size; idx-->0 ;)
        numbers[idx] = boost::math::prime(idx);
    return numbers;
}();
#endif

namespace {
#ifdef _rt_SRC_DIR
boost::filesystem::path PrimeListDir = _rt_SRC_DIR;
auto PrimeListPath = PrimeListDir / PRIME_LIST_FILENAME;
#endif
const auto PrimesBeginIterator = std::begin(Primes);
const auto PrimesEndIterator = std::end(Primes);
} // namespace

namespace omnn::rt {

// Utility function to find power
number_t power(number_t base, number_t exp,
                                     number_t mod) {
    number_t result = 1;
    while (exp > 0) {
        if (exp % 2 == 1)
            result = (result * base) % mod;
        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}

// Function to find gcd
number_t gcd(number_t a, number_t b) {
    while (b != 0) {
        number_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

// Check if a given number is prime
bool is_prime(const number_t& n) {
    if (n == 2 || n == 3)
        return true;
    if (n < 2 || n % 2 == 0)
        return false;

    number_t r, i;
    bool flag = true;
    for (r = 1; r < n; ++r) {
        if (gcd(r, n) != 1)
            return false;
        if (power(r, n - 1, n) != 1)
            return false;
    }

    return true;
}

void PrimeLookupTable::emplace() {
    auto num = back();
    while (!is_prime(++num))
        ;
    emplace_back(std::move(num));
}

void PrimeLookupTable::Generate(prime_idx_t idx)
{
    for (auto i = idx - size(); i-->0 ;)
        emplace();
}

auto NextToCheckForPrime() {
    boost::filesystem::ifstream inpt(PrimeListPath);
    char c;
    std::string s;
    inpt.seekg(0, std::ios::end);
    auto pos = inpt.tellg();
    do {
        pos -= 1;
        inpt.seekg(pos, std::ios::beg);
        inpt.get(c);
        if (std::isdigit(c))
            s.insert(s.begin(), c);
    } while (c != ',');
    assert(s.length() > 1);

    return ++number_t(s);
}

bool GrowPrime(const number_t& upto,
               std::function<bool(number_t)> is_prime)
{
    static number_t from = NextToCheckForPrime();
    auto prev = from - 1;
    if (upto <= prev)
        return {}; // another table is in use

    auto range = upto;
    range -= prev;
    auto chunks = std::thread::hardware_concurrency()
#ifdef OPENMIND_BUILD_GC
                    -1 // One thread left free for GC
#endif
        ;
    auto chunk = range;
    chunk /= chunks;
    if (from + chunk * chunks < upto) {
        ++chunk;
    }
    std::deque<std::future<std::string>> primining;
    std::cout << "new prime table target: " << upto << '(' << from + chunk * chunks << ')'<<std::endl;
    for (decltype(chunks) i = 0; i < chunks; ++i) {
        primining.emplace_back(std::async(std::launch::async, [=]() {
            std::stringstream ss;
            auto j = chunk;
            j *= i;
            j += from;
            auto up = j;
            up += chunk; // NOTE: preserving up type same as j. Expression (j+chunk)  type differs from j type.
            {
                static std::mutex m;
                std::lock_guard l(m);
                std::cout << '[' << j << ',' << up << ')';
            }
            for (; j < up; ++j) {
                if (is_prime(j)) {
                    std::cout << j << ',';
                    ss << j << ',';
                }
            }
            return std::move(ss.str());
        }));
    }

    auto i = -1;
    while (primining.size()) {
        auto line = primining.front().get();
        std::cout << "Chunk " << ++i << std::endl << line << std::endl << "\n chunk is ready. Appending." << std::endl;
        boost::filesystem::ofstream PrimesIncFile(PrimeListPath, std::ios_base::app);
        PrimesIncFile << std::endl << line;
        PrimesIncFile.close();
        primining.pop_front();
    }
    from++ = upto;
    return true;
}

const prime_t& prime(size_t idx) {
    return Primes[idx];
}

size_t primes() {
    return Primes.size() - 1;
}

prime_idx_t greatest_prime_idx(const number_t& bound) {
    auto lowerBoundIterator = std::lower_bound(PrimesBeginIterator, PrimesEndIterator, bound);
    if (*lowerBoundIterator != bound && lowerBoundIterator != PrimesBeginIterator)
    {
        ++lowerBoundIterator;
    }
    size_t idx = lowerBoundIterator - PrimesBeginIterator;
    return idx;
}

const prime_t& get_greatest_prime(const number_t& bound) {
    return prime(greatest_prime_idx(bound));
}

auto GetNextToCheckForPrimeToMine() {
    auto p = NextToCheckForPrime();
    boost::filesystem::ofstream PrimesIncFile(PrimeListPath, std::ios_base::app);
    PrimesIncFile << '\n';
    PrimesIncFile.close();
    return p;
}


namespace {
number_t factorial(const number_t& n) {
    auto result = n;
    if (n == 0)
        ++result;
    else
        for (auto i = n; --i > 1;)
            result *= i;
    return result;
}
}

void MineNextPrime() {

    static auto next = NextToCheckForPrime();
    static auto i = next; --i;
    static auto prev = i; --prev;
    static auto nFact = factorial(prev);

	for (;
		i < boost::math::max_prime;
		++prev, ++i, ++next)
	{
        nFact *= i;
        auto fastPrimeTest = (prev * (nFact % next));
        if (fastPrimeTest != 0) {
            auto prime = fastPrimeTest / i + 2;
            std::cout << prime << ',';

                //boost::filesystem::ofstream PrimesIncFile(PrimeListPath, std::ios_base::app);
                //PrimesIncFile << ',' << prime;
                //PrimesIncFile.close();
                break;
        }
    }
}

} // namespace omnn::rt
