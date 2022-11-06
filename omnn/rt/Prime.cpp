#include "Prime.h"
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
#ifndef OPENMIND_PRIME_TABLE_OM
#include <boost/tokenizer.hpp>
#endif

#include <rt/tasq.h>

#define PRIME_LIST_FILENAME "Primes.inc"
#ifdef OPENMIND_PRIME_TABLE_OM
static const boost::multiprecision::cpp_int Primes[] = {
#include "Primes.inc"
};
static const size_t PrimeItems = sizeof(Primes) / sizeof(Primes[0]);
#elif defined(OPENMIND_PRIME_TABLE_BOOST)
static constexpr size_t PrimeItems = boost::math::max_prime + 1;
#else
static_assert(!"Specify primes table");
#endif

namespace {
#ifdef rt_SRC_DIR
boost::filesystem::path PrimeListDir = rt_SRC_DIR;
auto PrimeListPath = PrimeListDir / PRIME_LIST_FILENAME;
#endif
} // namespace

namespace omnn::rt {

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

    return ++boost::multiprecision::cpp_int(s);
}

bool GrowPrime(const boost::multiprecision::cpp_int& upto,
               std::function<bool(boost::multiprecision::cpp_int)> is_prime) {
    static boost::multiprecision::cpp_int from = NextToCheckForPrime();
    auto prev = from - 1;
    if (upto <= prev)
        return {}; // another table is in use

    auto range = upto;
    range -= prev;
    auto chunks = std::thread::hardware_concurrency() - 1; // One thread left free for GC
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

const boost::multiprecision::cpp_int
#ifndef OPENMIND_PRIME_TABLE_BOOST
&
#endif
 prime(size_t idx) {
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
#ifdef OPENMIND_PRIME_TABLE_OM
    return Primes[idx];
#elif defined(OPENMIND_PRIME_TABLE_BOOST)
    return boost::math::prime(boost::numeric_cast<uint32_t>(idx));
#else
    static_assert(!"Specify primes table");
#endif // OPENMIND_PRIME_TABLE_BOOST
}

size_t primes() {
#ifdef OPENMIND_PRIME_TABLE_OM
    return PrimeItems - 1;
#elif defined(OPENMIND_PRIME_TABLE_BOOST)
    return boost::math::max_prime;
#else
    static_assert(!"Specify primes table");
#endif // OPENMIND_PRIME_TABLE_BOOST
}

auto GetNextToCheckForPrimeToMine() {
    auto p = NextToCheckForPrime();
    boost::filesystem::ofstream PrimesIncFile(PrimeListPath, std::ios_base::app);
    PrimesIncFile << '\n';
    PrimesIncFile.close();
    return p;
}


namespace {
boost::multiprecision::cpp_int factorial(const boost::multiprecision::cpp_int& n) {
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
