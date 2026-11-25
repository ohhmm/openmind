#pragma once

#include <cassert>
#include <cstddef>
#include <span>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#endif
#if defined(_M_X64) && defined(_MSC_VER)
#include <intrin.h>
#endif

namespace omnn::rt {

using Word = unsigned long long;

namespace detail {

inline Word AddWord(Word left, Word right, Word carry, Word& result) noexcept
{
#if defined(__x86_64__) || defined(_M_X64)
    return _addcarry_u64(static_cast<unsigned char>(carry), left, right, &result);
#else
    const Word sum = left + right;
    result = sum + carry;
    return (sum < left) | (result < sum);
#endif
}

} // namespace detail

// Add equal-length, least-significant-word-first operands.
// Result may alias either operand exactly. The return value is the final carry.
inline Word AddWords(std::span<Word> result, std::span<const Word> left,
                     std::span<const Word> right, Word carry = 0) noexcept
{
    assert(result.size() == left.size() && left.size() == right.size());
    assert(carry <= 1);
    for (std::size_t i = 0; i < result.size(); ++i)
        carry = detail::AddWord(left[i], right[i], carry, result[i]);
    return carry;
}

namespace detail {

template<Word Boundary>
inline bool AdvanceWords(std::span<Word> words) noexcept
{
    // A magnitude increment wraps all-ones words. A decrement wraps zero words.
    constexpr Word step = Boundary ? 1 : ~Word{0};
    std::size_t wrapped = 0;
#if defined(__x86_64__) && defined(__LP64__) && (defined(__GNUC__) || defined(__clang__))
    if (words.empty())
        return true;

    auto* cursor = words.data();
    auto remaining = words.size();
    unsigned char matched;
    asm volatile("repe scasq\n\tsete %0"
                 : "=q"(matched), "+D"(cursor), "+c"(remaining)
                 : "a"(Boundary)
                 : "cc", "memory");
    wrapped = words.size() - remaining - (matched == 0);

    cursor = words.data();
    auto count = wrapped;
    asm volatile("rep stosq"
                 : "+D"(cursor), "+c"(count)
                 : "a"(~Boundary)
                 : "memory");
#elif defined(_M_X64) && defined(_MSC_VER)
    while (wrapped < words.size() && words[wrapped] == Boundary)
        ++wrapped;
    __stosq(words.data(), ~Boundary, wrapped);
#else
    while (wrapped < words.size() && words[wrapped] == Boundary) {
        words[wrapped] = ~Boundary;
        ++wrapped;
    }
#endif
    if (wrapped == words.size())
        return true;
    words[wrapped] += step;
    return false;
}

} // namespace detail

// Increment least-significant-word-first operands and return the final carry.
inline bool IncrementWords(std::span<Word> words) noexcept
{
    return detail::AdvanceWords<~Word{0}>(words);
}

// Decrement a nonzero, least-significant-word-first magnitude.
inline void DecrementWords(std::span<Word> words) noexcept
{
    [[maybe_unused]] const bool borrow = detail::AdvanceWords<0>(words);
    assert(!borrow);
}

} // namespace omnn::rt
