#pragma once
#include <unordered_set>

#include <omnn/rt/strhash.hpp>


namespace omnn::math {

    class Valuable;

class StateProxyComparator
    : public ::omnn::rt::OuterParenthesesIgnoringStringsComparator
{
    using base = ::omnn::rt::OuterParenthesesIgnoringStringsComparator;

public:
    using tokens_collection_t =
        ::std::unordered_multiset<::std::string_view, ::omnn::rt::HashStrOmitOuterBrackets, StateProxyComparator>;

private:
    const Valuable* val;
    static thread_local const Valuable* state;

    [[nodiscard]]
    static tokens_collection_t
    TokenizeStringViewToMultisetKeepBracesWithStateProxyComparator(const ::std::string_view& str, char delimiter);

public:
    StateProxyComparator();
    StateProxyComparator(const Valuable*);
    StateProxyComparator(const Valuable&);
    ~StateProxyComparator();

    [[nodiscard]]
    bool operator()(const std::string_view& str1, const std::string_view& str2) const;

    [[nodiscard]]
    tokens_collection_t TokenizeStringViewToMultisetKeepBraces(const std::string_view& str, char delimiter) const;
};

}
