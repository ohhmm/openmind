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

} // namespace omnn::rt
