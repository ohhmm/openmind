#pragma once
#include <deque>


namespace omnn::rt {

template <typename ValueT>
class Lookup
    : public std::deque<ValueT>
{
    using base_t = std::deque<ValueT>;

    static const ValueT Empty;

protected:
    using value_t = ValueT;
    using size_type = base_t::size_type;

    virtual void Generate(size_type index) = 0;

public:
    using base_t::base_t;
    using base_t::size;
    using base_t::begin;
    using base_t::end;
    using base_t::resize;

    const auto& operator[](size_type index) {
        if (index >= size()) {
            Generate(index);
        }
        auto& item = base_t::operator[](index);
        if (item == Empty) {
            Generate(index);
        }
        return item;
    }
};

template <typename ValueT> const ValueT Lookup<ValueT>::Empty = {};

}
