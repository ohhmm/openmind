#include "Factors.hpp"

#include <boost/numeric/conversion/cast.hpp>


using namespace omnn::rt;


void FactorsLookupTable::Generate(size_type num)
{
    if (num >= size())
        resize(num + 1);

    auto& factors = base_container_t::operator[](num);
    for (prime_idx_t i = 0; num > 1; ++i) {
        auto prn = boost::numeric_cast<size_type>(prime(i));
        auto divisor = num % prn == 0;
        if (divisor) {
            do {
                num /= prn;
                divisor = num % prn == 0;
            } while (divisor);
            factors.emplace(std::move(prn));
        }
    }
}

const FactorsLookupTable::value_t& FactorsLookupTable::Factors(const number_t& num) {
    static FactorsLookupTable Instance; // TODO: const section chunk of preinitialized lookup concatinated with ranges::view to runtime extended lookup
    return Instance[boost::numeric_cast<size_type>(num < 0 ? -num : num)];
}
