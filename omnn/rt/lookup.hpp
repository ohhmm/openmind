#pragma once
#include <vector>


namespace omnn::rt {

template <typename ValueT>
class Lookup
    : public std::vector<ValueT>
{
    using base_t = std::vector<ValueT>;
    using value_t = ValueT;
    using size_type = base_t::size_type;

    size_type lookup = [this]() { return size(); }();
    static const ValueT Empty;

protected:
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
