#pragma once
#include <map>
#include <string>
#include <string_view>
#include <utility>


namespace omnn::rt {

    using brackets_map_t = std::map<size_t, size_t>;

[[nodiscard]]
brackets_map_t BracketsMap(const std::string_view&);

constexpr std::string_view& Trim(std::string_view& str) {
    str.remove_prefix(::std::min(str.find_first_not_of(" \t\r\v\n"), str.size()));
    str.remove_suffix((str.size() - 1) - ::std::min(str.find_last_not_of(" \t\r\v\n"), str.size() - 1));
    return str;
}

std::string_view Trim(std::string&);

[[nodiscard]]
auto OmitOuterBrackets(auto& str) {
    brackets_map_t bracketsmap;
    bool outerBracketsDetected;
    do {
        outerBracketsDetected = {};
        Trim(str);
        bracketsmap = BracketsMap(str);
        auto l = str.length();
        auto first = bracketsmap.find(0);
        outerBracketsDetected = first != bracketsmap.end() && first->second == l - 1;
        if (outerBracketsDetected)
            str = str.substr(1, l - 2);
    } while (outerBracketsDetected);
    return bracketsmap;
}

[[nodiscard]]
std::string SolidStr(std::string);

[[nodiscard]]
std::string Solid(std::string);

[[nodiscard]]
std::string Solid(const std::string_view&);

struct HashStrOmitOuterBrackets
    : public std::hash<std::string_view>
{
    [[nodiscard]] size_t operator()(std::string_view) const;
};

struct HashStrIgnoringAnyParentheses {
    [[nodiscard]] size_t operator()(const std::string_view&) const;
};

struct OuterParenthesesIgnoringStringsComparator {
    [[nodiscard]] constexpr bool operator()(std::string_view str1, std::string_view str2) const {
        auto equal = str1 == str2;
        if (!equal) {
            (void)OmitOuterBrackets(str1);
            (void)OmitOuterBrackets(str2);
            equal = str1 == str2;
        }
        return equal;
    }
};

}
