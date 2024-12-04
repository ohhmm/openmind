#pragma once
#include <vector>


namespace omnn::rt {

template <typename ValueT>
class Lookup
    : public std::vector<ValueT>
{
    using base_t = std::vector<ValueT>;
    using size_type = base_t::size_type;

    size_type lookup = [this]() { return size(); }();
    static constexpr ValueT Empty = ValueT();

protected:
    virtual ValueT Generate(size_type index) = 0;

public:
    using base_t::base_t;
    using base_t::size;

    const auto& operator[](size_type index) {
        if (index >= lookup) {
            if (index >= size()) {
                resize(index+1);
            }
        }
        auto& item = base_t::operator[](index);
        if (item == Empty) {
            item = Generate(index);
        }
        return item;
    }
};

}
