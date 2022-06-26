#pragma once
#include <omnn/math/Variable.h>
#include <omnn/math/DuoValDescendant.h>

namespace omnn::math {

/// This implementation objects kept if it is surd, otherwise it optimized out.
class PrincipalSurd /// Also known as principal root of Nth index
        : public DuoValDescendant<PrincipalSurd>
{
    using base = DuoValDescendant<PrincipalSurd>;

protected:
    std::ostream& print(std::ostream&) const override;
    std::ostream& print_sign(std::ostream&) const override;

public:
    using base::base;
    PrincipalSurd(const Valuable& radicand, const Valuable& index = 2);

	bool IsRadical() const override { return true; }
    bool IsPrincipalSurd() const override { return true; }

	std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable& v) const;

	void optimize() override;
    
    std::ostream& code(std::ostream&) const override;
};

}
