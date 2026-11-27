#include "AsmInt.h"

#include <algorithm>
#include <stdexcept>
#include <limits>

namespace omnn::rt {

AsmInt::AsmInt(const boost::multiprecision::cpp_int& value)
{
    if (value == 0)
        return;

    int n = 1;
    while (true) {
        boost::multiprecision::cpp_int half = boost::multiprecision::cpp_int(1) << (64 * n - 1);
        boost::multiprecision::cpp_int lower = -half;
        boost::multiprecision::cpp_int upper = half - 1;
        if (value >= lower && value <= upper)
            break;
        ++n;
    }

    boost::multiprecision::cpp_int mod = boost::multiprecision::cpp_int(1) << (64 * n);
    boost::multiprecision::cpp_int v = value % mod;
    if (v < 0)
        v += mod;

    constexpr Word mask = std::numeric_limits<Word>::max();
    for (int i = 0; i < n; ++i) {
        push_back(static_cast<Word>(v & mask));
        v >>= std::numeric_limits<Word>::digits;
    }
}

AsmInt& AsmInt::operator++()
{
    return *this += Word(1);
}

AsmInt& AsmInt::operator+=(const AsmInt& other)
{
    auto signFill = [](const AsmInt& v) -> Word {
        if (v.empty())
            return Word(0);
        return (v.back() >> 63) ? std::numeric_limits<Word>::max() : Word(0);
    };

    std::size_t m = std::max(size(), other.size()) + 1;

    std::vector<Word> a(m, signFill(*this));
    std::vector<Word> b(m, signFill(other));
    for (std::size_t i = 0; i < size(); ++i)
        a[i] = (*this)[i];
    for (std::size_t i = 0; i < other.size(); ++i)
        b[i] = other[i];

    assign(a.begin(), a.end());

    unsigned char carry;
#if defined(__x86_64__) && !defined(_MSC_VER) && __SIZEOF_POINTER__ == 8
    auto* pa = data();
    auto* pb = b.data();
    auto count = m;
    asm volatile(
        "clc\n\t"
        "jrcxz 2f\n\t"
        "1: movq (%[pb]), %%rax\n\t"
        "adcq %%rax, (%[pa])\n\t"
        "leaq 8(%[pa]), %[pa]\n\t"
        "leaq 8(%[pb]), %[pb]\n\t"
        "loop 1b\n\t"
        "2: setc %[carry]"
        : [pa] "+r"(pa), [pb] "+r"(pb), "+c"(count), [carry] "=qm"(carry)
        :
        : "cc", "memory", "rax");
#else
    carry = 0;
    constexpr Word maxWord = std::numeric_limits<Word>::max();
    for (std::size_t i = 0; i < m; ++i) {
        Word x = (*this)[i];
        Word y = b[i];
        bool carryIn = carry != 0;
        bool c1 = (x > maxWord - y);
        Word sum = x + y;
        bool c2 = carryIn && (sum > maxWord - (carryIn ? 1 : 0));
        sum += carryIn ? 1 : 0;
        (*this)[i] = sum;
        carry = (c1 || c2) ? 1 : 0;
    }
#endif
    (void)carry;

    // Trim redundant sign-extension words.
    while (size() > 1) {
        Word top = back();
        Word next = (*this)[size() - 2];
        bool redundant = (top == 0 && !(next >> 63)) || (top == std::numeric_limits<Word>::max() && (next >> 63));
        if (redundant)
            pop_back();
        else
            break;
    }
    if (size() == 1 && back() == 0)
        clear();

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
    return *this += AsmInt(static_cast<boost::multiprecision::cpp_int>(other));
}

AsmInt AsmInt::operator+(Word other) const
{
    AsmInt result = *this;
    result += other;
    return result;
}


AsmInt AsmInt::operator+(const boost::multiprecision::cpp_int& other) const
{
    return *this + AsmInt(other);
}

AsmInt& AsmInt::operator+=(const boost::multiprecision::cpp_int& other)
{
    return *this += AsmInt(other);
}

AsmInt::operator boost::multiprecision::cpp_int() const
{
    boost::multiprecision::cpp_int result = 0;
    for (auto it = rbegin(); it != rend(); ++it) {
        result <<= std::numeric_limits<Word>::digits;
        result += *it;
    }
    if (!empty() && (back() >> 63))
        result -= boost::multiprecision::cpp_int(1) << (std::numeric_limits<Word>::digits * size());
    return result;
}

} // namespace omnn::rt
