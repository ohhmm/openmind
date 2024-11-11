#include <utility>
#include <algorithm>
#include "Sum.h"
#include "Valuable.h"
#include "Exponentiation.h"

namespace omnn {
namespace math {

    std::pair<bool,Valuable> Sum::IsSummationSimplifiable(const Valuable& v) const {
        std::pair<bool, Valuable> is{{}, v + *this};

        // Check if we're dealing with bit operation formulas
        bool has_bit_ops = false;
        for (const auto& m : members) {
            if (m.IsExponentiation()) {
                const auto& exp = m.as<Exponentiation>();
                const auto& base = exp.getBase();
                if (base == -1) {
                    has_bit_ops = true;
                    break;
                }
            }
        }

        if (has_bit_ops) {
            // For bit operations, we need to preserve exact exponent relationships
            auto common_vars = getCommonVars();
            auto v_vars = v.getCommonVars();

            // Check if the variables match exactly for bit operation formulas
            bool exact_match = true;
            for (const auto& var : common_vars) {
                auto it = v_vars.find(var.first);
                if (it == v_vars.end() || it->second != var.second) {
                    exact_match = false;
                    break;
                }
            }

            is.first = exact_match && (is.second.Complexity() < Complexity() + v.Complexity());
        } else {
            // For non-bit-operation cases, use the GCD-based approach
            is.first = is.second.Complexity() < Complexity() + v.Complexity();
        }

        return is;
    }

}} // namespace omnn::math
