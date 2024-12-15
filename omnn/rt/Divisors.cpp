#include "Divisors.hpp"

#include "Factors.hpp"

#include <boost/numeric/conversion/cast.hpp>


using namespace omnn::rt;


void DivisorsLookupTable::Generate(size_type num) {
    if (num >= size())
        resize(num + 1);

    auto& factors = FactorsLookupTable::Factors(num);
    auto& divisors = base_container_t::operator[](num);
    divisors = factors;
    auto half = num >> 1;
    auto factorsBegin = factors.begin();
    auto factorsEnd = factors.end();
    if (*factorsBegin == 1) {
        ++factorsBegin;
    }
    for (auto factor = factorsBegin; factor != factorsEnd; ++factor) {
        auto begin = divisors.begin();
        auto end = divisors.end();
        divisors_set_t new_divisors;
        for (auto exp = *factor; exp <= half; exp *= *factor) {

            for (auto it = begin;
                it != end && *it <= half;
                ++it)
            {
                auto candidate = *it * exp;
                if (num % candidate == 0) {
                    new_divisors.insert(candidate);
                }
            }
        }

        divisors.merge(std::move(new_divisors));
    }
}

const DivisorsLookupTable::value_t& DivisorsLookupTable::Divisors(const number_t& num) {
    static DivisorsLookupTable Instance; // TODO: const section chunk of preinitialized lookup concatinated with ranges::view to runtime extended lookup
    return Instance[boost::numeric_cast<size_type>(num < 0 ? -num : num)];
}
