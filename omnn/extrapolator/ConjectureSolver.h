//
// Created by Sergej Krivonos on 07.11.26.
//
#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace omnn::math {

/// Finds the simplest exact formula consistent with named samples.
/// Relationship patterns are tried from the simplest upward: equality,
/// direct proportion, inverse proportion, then small integer-coefficient
/// expressions in the other named values.
class ConjectureSolver {
public:
    using row_t = std::map<std::string, std::int64_t>;

    static ConjectureSolver Parse(std::string_view spec);

    void Add(row_t row);

    /// Expression for @p target in the other names. Empty when no exact
    /// conjecture is found within the searched patterns.
    std::string Solve(std::string_view target) const;

private:
    std::vector<row_t> rows;
};

} // namespace omnn::math
