#pragma once
#include <omnn/rt/Prime.h>

#include <set>


namespace omnn::rt {

    using factors_set_t = std::set<prime_t>;

class FactorsLookupTable
	: public Lookup<factors_set_t>
{
    using base_t = omnn::rt::Lookup<factors_set_t>;
    using value_t = base_t::value_t;

protected:
    using base_t::base_t;
    void Generate(size_type number) override;

public:
    using base_t::operator[];
    static const value_t& Factors(const number_t&);
};

} // namespace omnn::rt
