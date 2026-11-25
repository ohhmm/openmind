#pragma once

#include "add_words.hpp"

#include <cstdint>
#include <vector>

namespace omnn::rt {

class AsmInt : private std::vector<Word> {
    using Base = std::vector<Word>;
    bool negative_ = false;

    void Normalize() noexcept;
    void AddMagnitude(std::span<const Word> right);
    void SubtractMagnitude(std::span<const Word> right) noexcept;

public:
    AsmInt() = default;
    explicit AsmInt(std::int64_t value);
    // The words encode the magnitude, least significant first.
    explicit AsmInt(std::span<const Word> words, bool negative = false);
    AsmInt(const AsmInt&) = default;
    AsmInt& operator=(const AsmInt&) = default;
    AsmInt(AsmInt&& other) noexcept;
    AsmInt& operator=(AsmInt&& other) noexcept;

    auto begin() const noexcept { return cbegin(); }
    auto end() const noexcept { return cend(); }
    std::span<const Word> Words() const noexcept { return {data(), size()}; }
    bool IsNegative() const noexcept { return negative_; }

    AsmInt& operator+=(const AsmInt& right);
    AsmInt& operator++();

    friend AsmInt operator+(AsmInt left, const AsmInt& right)
    {
        left += right;
        return left;
    }
    bool operator==(const AsmInt&) const = default;
};

} // namespace omnn::rt
