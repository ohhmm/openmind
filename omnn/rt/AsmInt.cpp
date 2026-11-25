#include "AsmInt.h"

#include <algorithm>
#include <utility>

namespace omnn::rt {

AsmInt::AsmInt(std::int64_t value) : negative_(value < 0)
{
    if (value)
        push_back(negative_ ? Word{0} - static_cast<Word>(value) : static_cast<Word>(value));
}

AsmInt::AsmInt(std::span<const Word> words, bool negative)
    : Base(words.begin(), words.end()), negative_(negative)
{
    Normalize();
}

AsmInt::AsmInt(AsmInt&& other) noexcept
    : Base(std::move(other)), negative_(std::exchange(other.negative_, false))
{
    other.clear();
}

AsmInt& AsmInt::operator=(AsmInt&& other) noexcept
{
    if (this != &other) {
        Base::operator=(std::move(other));
        negative_ = std::exchange(other.negative_, false);
        other.clear();
    }
    return *this;
}

void AsmInt::Normalize() noexcept
{
    while (!empty() && !back())
        pop_back();
    if (empty())
        negative_ = false;
}

void AsmInt::AddMagnitude(std::span<const Word> right)
{
    if (right.empty())
        return;

    const bool self = right.data() == data();
    const auto resultSize = std::max(size(), right.size());
    reserve(resultSize + 1);
    resize(resultSize);
    if (self)
        right = Words();

    auto common = std::span<Word>(data(), size()).first(right.size());
    auto carry = AddWords(common, common, right);
    for (auto& word : std::span<Word>(data(), size()).subspan(right.size())) {
        if (!carry)
            break;
        carry = ++word == 0;
    }
    if (carry)
        push_back(carry);
}

void AsmInt::SubtractMagnitude(std::span<const Word> right) noexcept
{
    Word borrow = 0;
    for (std::size_t i = 0; i < right.size(); ++i) {
        const Word word = (*this)[i];
        const Word difference = word - right[i];
        (*this)[i] = difference - borrow;
        borrow = (word < right[i]) | (difference < borrow);
    }
    for (auto& word : std::span<Word>(data(), size()).subspan(right.size())) {
        if (!borrow)
            break;
        borrow = word-- == 0;
    }
    Normalize();
}

AsmInt& AsmInt::operator+=(const AsmInt& right)
{
    if (negative_ == right.negative_) {
        AddMagnitude(right.Words());
    } else if (size() > right.size() ||
               (size() == right.size() &&
                !std::lexicographical_compare(rbegin(), rend(),
                                              right.rbegin(), right.rend()))) {
        SubtractMagnitude(right.Words());
    } else {
        AsmInt result(right);
        result.SubtractMagnitude(Words());
        *this = std::move(result);
    }
    return *this;
}

AsmInt& AsmInt::operator++()
{
    if (negative_) {
        DecrementWords({data(), size()});
        Normalize();
    } else {
        reserve(size() + 1);
        if (IncrementWords({data(), size()}))
            push_back(1);
    }
    return *this;
}

} // namespace omnn::rt
