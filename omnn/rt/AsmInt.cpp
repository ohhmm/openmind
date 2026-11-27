#include "AsmInt.h"

#include <algorithm>
#include <limits>

namespace omnn::rt {

AsmInt& AsmInt::operator++()
{
    // Reserve any required growth before the carry loop mutates the value.
    if (size() == capacity() &&
        std::all_of(cbegin(), cend(), [](Word word) {
            return word == std::numeric_limits<Word>::max();
        }))
        reserve(size() + 1);

    unsigned char carry;
#if defined(__x86_64__) && !defined(_MSC_VER) && __SIZEOF_POINTER__ == 8
    auto* word = data();
    auto count = size();
    asm volatile(
        "stc\n\t"
        "jrcxz 2f\n\t"
        "1: adcq $0, (%[word])\n\t"
        "jnc 2f\n\t"
        "leaq 8(%[word]), %[word]\n\t"
        "decq %%rcx\n\t"
        "jnz 1b\n\t"
        "2: setc %[carry]"
        : [word] "+r"(word), "+c"(count), [carry] "=qm"(carry)
        :
        : "cc", "memory");
#else
    carry = 1;
    for (auto& word : *this) {
        if (++word) {
            carry = 0;
            break;
        }
    }
#endif

    if (carry)
        push_back(1);

    return *this;
}

AsmInt& AsmInt::operator+=(const AsmInt& other)
{
    if (size() == capacity() && capacity() > 0)
        reserve(size() + 1);

    if (other.size() > size())
        resize(other.size(), 0);

    AsmInt rhs = other;
    if (rhs.size() < size())
        rhs.resize(size(), 0);

    unsigned char carry;
#if defined(__x86_64__) && !defined(_MSC_VER) && __SIZEOF_POINTER__ == 8
    auto* a = data();
    auto* b = rhs.data();
    auto count = size();
    asm volatile(
        "clc\n\t"
        "jrcxz 2f\n\t"
        "1: movq (%[b]), %%rax\n\t"
        "adcq %%rax, (%[a])\n\t"
        "leaq 8(%[a]), %[a]\n\t"
        "leaq 8(%[b]), %[b]\n\t"
        "loop 1b\n\t"
        "2: setc %[carry]"
        : [a] "+r"(a), [b] "+r"(b), "+c"(count), [carry] "=qm"(carry)
        :
        : "cc", "memory", "rax");
#else
    carry = 0;
    constexpr Word maxWord = std::numeric_limits<Word>::max();
    for (std::size_t i = 0; i < size(); ++i) {
        Word a = (*this)[i];
        Word b = rhs[i];
        bool carryIn = carry != 0;
        bool c1 = (a > maxWord - b);
        Word sum = a + b;
        bool c2 = carryIn && (sum > maxWord - (carryIn ? 1 : 0));
        sum += carryIn ? 1 : 0;
        (*this)[i] = sum;
        carry = (c1 || c2) ? 1 : 0;
    }
#endif

    if (carry)
        push_back(1);

    return *this;
}

AsmInt AsmInt::operator+(const AsmInt& other) const
{
    AsmInt result = *this;
    result += other;
    return result;
}

AsmInt& AsmInt::operator+=(Word other)
{
    AsmInt rhs;
    rhs.push_back(other);
    return *this += rhs;
}

AsmInt AsmInt::operator+(Word other) const
{
    AsmInt result = *this;
    result += other;
    return result;
}

AsmInt::operator boost::multiprecision::cpp_int() const
{
    boost::multiprecision::cpp_int result = 0;
    for (auto it = rbegin(); it != rend(); ++it) {
        result <<= std::numeric_limits<Word>::digits;
        result += *it;
    }
    return result;
}

} // namespace omnn::rt
