#pragma once

#include <cstdint>
#include <vector>

namespace omnn::rt {

using Word = std::uint64_t;

// Unsigned words in least-significant-first order.
class AsmInt : public std::vector<Word> {
public:
    using std::vector<Word>::vector;

    AsmInt& operator++();
};

} // namespace omnn::rt
