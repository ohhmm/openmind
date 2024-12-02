#include "strhash.hpp"

#include <algorithm>
#include <stack>
#include <type_traits>


namespace omnn::rt {

[[nodiscard]]
brackets_map_t BracketsMap(const std::string_view& s) {
    auto l = s.length();
    using index_t = decltype(l);
    std::stack<index_t> st;
    std::map<index_t, index_t> bracketsmap;
    index_t c = 0;
    while (c < l) {
        if (s[c] == '(')
            st.push(c);
        else if (s[c] == ')') {
            if (st.empty()) {
                throw "parentheses relation mismatch";
            }
            bracketsmap.emplace(st.top(), c);
            st.pop();
        }
        ++c;
    }
    if (!st.empty())
        throw "parentheses relation mismatch";
    return bracketsmap;
}

std::string_view Trim(std::string& str) {
    auto view = std::string_view(str);
    view = Trim(view);
    str = view;
    return str;
}

[[nodiscard]]
std::string SolidStr(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    (void)OmitOuterBrackets(str);
    if (str.ends_with("+0")) {
        str.resize(str.length() - 2);
        (void)OmitOuterBrackets(str);
    }
    return str;
}

[[nodiscard]]
std::string Solid(std::string str) {
    return SolidStr(str);
}

[[nodiscard]]
std::string Solid(const std::string_view& str) {
    return SolidStr(std::string(str));
}

[[nodiscard]] size_t HashStrOmitOuterBrackets::operator()(std::string_view s) const {
    auto str = s;
    (void)OmitOuterBrackets(str);
    return std::hash<std::string_view>::operator()(str);
}

[[nodiscard]] size_t HashStrIgnoringAnyParentheses::operator()(const std::string_view& str) const {
    size_t hash = 0;
    for (auto ch : str) {
        if (ch != '(' && ch != ')')
            hash ^= ch;
    }
    return hash;
}

} // namespace omnn::rt
