#include <cmath>
#include "Logarithm.h"

namespace omnn {
namespace math {

	std::ostream& Logarithm::print(std::ostream& out) const
	{
		out << "log(" << _1 << ", " << _2 << ')';
		return out;
	}

	std::ostream& Logarithm::print_sign(std::ostream& out) const
	{
		out << 'l';
		return out;
	}

	constexpr auto Logarithm::GetBinaryOperationLambdaTemplate() {
        return [](const auto& base, const auto& target) {
			return ::std::log(target) / ::std::log(base);
		};
	}

	void Logarithm::optimize()
	{
		
	}

	Valuable& Logarithm::operator +=(const Valuable& v)
	{
		IMPLEMENT
		return base::operator +=(v);
	}

	Valuable& Logarithm::operator *=(const Valuable& v)
	{
		IMPLEMENT
		return base::operator *=(v);
	}

	bool Logarithm::MultiplyIfSimplifiable(const Valuable& v)
	{
        IMPLEMENT
		return base::MultiplyIfSimplifiable(v);
	}

	std::pair<bool, Valuable> Logarithm::IsMultiplicationSimplifiable(const Valuable& v) const
	{
        IMPLEMENT
        return base::IsMultiplicationSimplifiable(v);
	}

	bool Logarithm::SumIfSimplifiable(const Valuable& v)
	{
        IMPLEMENT
        return base::SumIfSimplifiable(v);
	}

	std::pair<bool, Valuable> Logarithm::IsSummationSimplifiable(const Valuable& v) const
	{ IMPLEMENT }


}  // namespace math
}  // namespace omnn
