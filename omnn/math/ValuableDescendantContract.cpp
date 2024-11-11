//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Exponentiation.h"
#include "Fraction.h"
#include "Modulo.h"
#include "PrincipalSurd.h"
#include "Product.h"
#include "Sum.h"
#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <set>

namespace omnn{
namespace math {

Valuable& ValuableDescendantBase::operator^=(const Valuable& v)
{
    return Become(Exponentiation(*this,v));
	// FIXME: TODO: return Become(Exponentiation(std::move(*this), v));
}

Valuable& ValuableDescendantBase::operator+=(const Valuable &v)
{
    return Become(Sum{*this,v});
}

Valuable& ValuableDescendantBase::operator*=(const Valuable &v)
{
    return Become(Product{*this,v});
}

Valuable& ValuableDescendantBase::operator/=(const Valuable &v)
{
    return Become(Fraction(*this,v));
}

Valuable& ValuableDescendantBase::operator%=(const Valuable &v)
{
    return Become(Modulo(*this,v));
}

Valuable ValuableDescendantBase::operator -() const
{
    return *this * -1;
}

void ValuableDescendantBase::Values(const std::function<bool(const Valuable&)> &fun) const {
    auto imv = IsMultival();
    if (imv == YesNoMaybe::No)
        fun(*this);
    else
        LOG_AND_IMPLEMENT(*this << " Values method");
}

Valuable ValuableDescendantBase::Univariate() const {
	auto uni = constants::one;
	for (auto &branch : Distinct()) {
		uni.logic_or(branch);
	}
	return uni;
}

Valuable& ValuableDescendantBase::sq() {
    return Become(Exponentiation{*this, 2});
}

Valuable ValuableDescendantBase::Sqrt() const {
    return PrincipalSurd{*this};
}

bool ValuableDescendantBase::IsComesBefore(const Valuable& v) const {
    BOOST_TEST_MESSAGE("Comparing expressions:");
    BOOST_TEST_MESSAGE("this: " << this->str());
    BOOST_TEST_MESSAGE("other: " << v.str());

    // First try to evaluate both expressions numerically
    auto thisVal = this->varless();
    auto otherVal = v.varless();

    // Helper function to get rational value
    auto getRationalValue = [](const Valuable& val) -> std::pair<bool, a_rational> {
        try {
            return {true, static_cast<a_rational>(val.varless())};
        } catch (...) {
            return {false, a_rational{0}};
        }
    };

    // For expressions with multiple solutions (like square roots)
    if (IsMultival() == YesNoMaybe::Yes || v.IsMultival() == YesNoMaybe::Yes) {
        std::set<Valuable> thisVals, otherVals;
        this->Values([&](const Valuable& val) { thisVals.insert(val); return true; });
        v.Values([&](const Valuable& val) { otherVals.insert(val); return true; });

        if (!thisVals.empty() && !otherVals.empty()) {
            // Get all values and their string representations
            std::vector<std::pair<a_rational, std::string>> thisRatVals, otherRatVals;

            for (const auto& val : thisVals) {
                auto [success, rat] = getRationalValue(val);
                if (success) {
                    thisRatVals.emplace_back(rat, val.str());
                }
            }

            for (const auto& val : otherVals) {
                auto [success, rat] = getRationalValue(val);
                if (success) {
                    otherRatVals.emplace_back(rat, val.str());
                }
            }

            // Sort by value first, then by string representation
            auto sortPairs = [](auto& pairs) {
                std::sort(pairs.begin(), pairs.end(),
                    [](const auto& a, const auto& b) {
                        return a.first == b.first ? a.second < b.second : a.first < b.first;
                    });
            };

            sortPairs(thisRatVals);
            sortPairs(otherRatVals);

            // Compare the first (smallest) values
            if (!thisRatVals.empty() && !otherRatVals.empty()) {
                const auto& [thisRat, thisStr] = thisRatVals.front();
                const auto& [otherRat, otherStr] = otherRatVals.front();

                return thisRat == otherRat ? thisStr < otherStr : thisRat < otherRat;
            }
        }
    }

    // If both expressions can be evaluated to simple values
    if (thisVal.IsSimple() && otherVal.IsSimple()) {
        auto [thisSuccess, thisRat] = getRationalValue(*this);
        auto [otherSuccess, otherRat] = getRationalValue(v);

        if (thisSuccess && otherSuccess) {
            return thisRat == otherRat ? this->str() < v.str() : thisRat < otherRat;
        }
    }

    // For expressions that can't be evaluated numerically,
    // use canonical form
    return this->str() < v.str();
}

Valuable::universal_lambda_t ValuableDescendantBase::CompileIntoLambda(variables_for_lambda_t) const {
    LOG_AND_IMPLEMENT("Implement CompileIntoLambda: " << *this);
}

} // namespace math
} // namespace omnn
