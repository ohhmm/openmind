#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include <cstdint>
#include <vector>

namespace omnn::rt {

using Word = std::uint64_t;

// Unsigned words in least-significant-first order.
class AsmInt : public std::vector<Word> {
public:
    using std::vector<Word>::vector;

    AsmInt& operator++();
    AsmInt operator+(const AsmInt& other) const;
    AsmInt& operator+=(const AsmInt& other);
    AsmInt operator+(Word other) const;
    AsmInt& operator+=(Word other);
    explicit operator boost::multiprecision::cpp_int() const;
};

inline AsmInt operator+(Word lhs, const AsmInt& rhs) { return rhs + lhs; }

} // namespace omnn::rt
