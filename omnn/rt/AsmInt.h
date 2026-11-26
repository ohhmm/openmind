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
    explicit operator boost::multiprecision::cpp_int() const;
};

} // namespace omnn::rt
