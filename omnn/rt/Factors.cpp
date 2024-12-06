#include "Factors.hpp"

#include <boost/numeric/conversion/cast.hpp>


using namespace omnn::rt;


void FactorsLookupTable::Generate(size_type num)
{
    base_t::value_t factors;
    for (prime_idx_t i = 0; num > 1; ++i) {
        auto prn = boost::numeric_cast<size_type>(prime(i));
        auto divisor = num % prn == 0;
        if (divisor) {
            do {
                num /= prn;
                divisor = num % prn == 0;
            } while (divisor);
            factors.insert(std::move(prn));
        }
    }
    emplace_back(std::move(factors));
}

