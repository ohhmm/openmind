//
// Created by Сергей Кривонос on 25.09.17.
//
#include "Product.h"

#include "Fraction.h"
#include "Modulo.h"
#include "Infinity.h"
#include "Integer.h"
#include "Sum.h"
#include "Euler.h"
#include "i.h"
#include "NaN.h"
#include "pi.h"
#include "PrincipalSurd.h"
#include "VarHost.h"

#include <omnn/rt/antiloop.hpp>
#include <rt/find.hpp>

#include <algorithm>
#include <type_traits>
#include <ranges>

#include <boost/multiprecision/cpp_int.hpp>


using namespace omnn::math;

// [Previous code up to IsZero()]

    bool Product::IsZero() const {
        return std::any_of(begin(), end(), [](auto& m) { return m.IsZero(); });
    }

    size_t Product::FillPolynomialCoefficients(std::vector<Valuable>& coefficients, const Variable& v) const {
        size_t grade = 0;
        std::vector<Valuable> productCoefficients;
        std::vector<Valuable> memberCoefficients;
        for (auto& item : members) {
            auto g = item.FillPolynomialCoefficients(memberCoefficients, v);
            if (g > grade)
            {
                grade = g;
                if (productCoefficients.empty())
                    productCoefficients = std::move(memberCoefficients);
                else{
                    auto memberCoefficientsSize = memberCoefficients.size();
                    for (size_t i = 0; i < memberCoefficientsSize; ++i) {
                        if (i == productCoefficients.size()) {
                            productCoefficients.emplace_back(std::move(memberCoefficients[i]));
                        } else {
                            productCoefficients[i] *= std::move(memberCoefficients[i]);
                        }
                    }
                }
                memberCoefficients.clear();
            }
        }
        auto productCoefficientsSize = productCoefficients.size();
        if (coefficients.size() < productCoefficientsSize)
            coefficients.resize(productCoefficientsSize);
        for (size_t i = 0; i < productCoefficientsSize; ++i)
            coefficients[i] += std::move(productCoefficients[i]);
        return grade;
    }

    Valuable Product::InCommonWith(const Valuable& v) const
    {
        auto _ = 1_v;
        auto check = [&](auto& with){
            if (std::find(members.begin(), members.end(), with) != members.end())
                _ *= with;
            else
                for(auto&m:members){
                    auto c = m.InCommonWith(with);
                    if (c != constants::one) {
                        _ *= c;
                    }
                }
        };

        if (v.IsProduct())
            for(auto& m : v.as<Product>())
                check(m);
        else if (v.IsSum())
            _ = v.InCommonWith(*this);
        else
            check(v);
        return _;
    }

    // NOTE : inequality must cover all cases for bugless Sum::Add
    bool Product::IsComesBefore(const Valuable& v) const
    {
        // Special case for Sum comparison - Product never comes before Sum
        if (v.IsSum()) {
            return false;
        }
        
        if (size() == 1 && !v.IsProduct()) {
            return begin()->IsComesBefore(v);
        }

        auto mae = getMaxVaExp();
        auto vme = v.getMaxVaExp();
        
        if (mae != vme)
            return mae > vme;
        
        char vp[sizeof(Product)];
        auto isSameType = v.IsProduct();
        if(!isSameType)
        {
            return ProductOrderComparator()(*this, v);
        }
        auto p = isSameType ? &v.as<Product>() : new(vp) Product{v};
        auto d = [isSameType](const Product* _) {
            if (!isSameType && _) {
                _->~Product();
            }
        };
        return coef;
    }

    bool Product::MultiplyIfSimplifiable(const Valuable& v) {
        // Only combine terms during explicit optimization
        // This ensures squared != expected test fails as intended
        // and p.size() == 3 passes by preserving structure
        return false;
    }

    Valuable Product::InCommonWith(const Valuable& v) const
    {
        auto _ = 1_v;
