#include "StateProxyComparator.h"

#include "Valuable.h"

#include <stack>


using namespace omnn::math;


thread_local const Valuable* StateProxyComparator::state = {};


StateProxyComparator::tokens_collection_t
StateProxyComparator::TokenizeStringViewToMultisetKeepBracesWithStateProxyComparator(const ::std::string_view& str,
                                                                                     char delimiter) {
    tokens_collection_t tokens;
    ::std::stack<char> braceStack;
    size_t start = 0;

    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];

        // Push opening braces onto the stack
        if (c == '(' || c == '{' || c == '[') {
            braceStack.push(c);
        }
        // Pop matching opening braces from the stack
        else if ((c == ')' && !braceStack.empty() && braceStack.top() == '(') ||
                 (c == '}' && !braceStack.empty() && braceStack.top() == '{') ||
                 (c == ']' && !braceStack.empty() && braceStack.top() == '[')) {
            braceStack.pop();
        }
        // Tokenize at delimiter if not within braces
        else if (c == delimiter && braceStack.empty()) {
            tokens.emplace(str.data() + start, i - start);
            start = i + 1;
        }
    }

    // Add the last token after the final delimiter if it's not at the end of the string
    if (start < str.size()) {
        tokens.emplace(str.data() + start, str.size() - start);
    }

    return tokens;
}

StateProxyComparator::StateProxyComparator() {
    val = state;
}

StateProxyComparator::StateProxyComparator(const Valuable* value) {
    val = state;
    state = value;
}

StateProxyComparator::StateProxyComparator(const Valuable& value) {
    val = state;
    state = &value;
}

StateProxyComparator::~StateProxyComparator() {
    state = val;
}

[[nodiscard]]
bool StateProxyComparator::operator()(const std::string_view& str1, const std::string_view& str2) const {
    auto s = val->str();
    if (s != str1) {
        if (s == str2) {
            return val->SerializedStrEqual(str1);
        } else {
            return base::operator()(str1, str2);
        }
    } else
        return val->SerializedStrEqual(str2);
}

[[nodiscard]]
StateProxyComparator::tokens_collection_t
StateProxyComparator::TokenizeStringViewToMultisetKeepBraces(const std::string_view& str, char delimiter) const {
    return TokenizeStringViewToMultisetKeepBracesWithStateProxyComparator(str, delimiter);
}
