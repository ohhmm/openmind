//
// Created by Сергей Кривонос on 01.09.17.
//
#pragma once
#include <omnn/math/Exponentiation.h>
#include <omnn/math/Product.h>

namespace omnn{
namespace math {

namespace constants {
class ConstNameAdder;
} // namespace constants

template <class Chld>
    class Constant
        : public ValuableDescendantContract<Chld>
    {
        using base = ValuableDescendantContract<Chld>;

        static const/*init*/ constants::ConstNameAdder ConstNameToSerializationNamesMapAdder;

    protected:
        bool IsSubObject(const Valuable& o) const override {
            return this == &o.get();
        }

        std::ostream& print(std::ostream& out) const override {
            return out << Chld::SerializationName;
        }

    public:
        using base::base;
        static const/*init*/ Chld GlobalObject;

        [[nodiscard]]
        bool IsConstant() const noexcept override { return true; }
        [[nodiscard]]
        bool IsSimple() const noexcept override { return true; }
        [[nodiscard]]
        bool IsPolynomial(const Variable&) const noexcept override { return true; }
        [[nodiscard]]
        size_t FillPolynomialCoefficients(std::vector<Valuable>& coefficients, const Variable& v) const override {
            if (coefficients.empty())
                coefficients.resize(1);
            coefficients[0] += *this;
            return 0;
        }
        [[nodiscard]]
        const PrincipalSurd* PrincipalSurdFactor() const noexcept override { return {}; }
        [[nodiscard]]
        YesNoMaybe IsMultival() const noexcept override { return YesNoMaybe::No; }
        [[nodiscard]]
        YesNoMaybe IsRational() const noexcept override { return YesNoMaybe::No; }
        [[nodiscard]]
        bool is_optimized() const noexcept override { return true; }
        void optimize() noexcept override {}

        const Variable* FindVa() const noexcept override {
            return {};
        }

        Valuable* Clone() const noexcept override { return new Chld(); }

        [[nodiscard]]
        bool HasVa(const Variable& va) const noexcept override {
            return {};
        }

        void CollectVa(std::set<Variable>&) const override
        { }
        void CollectVaNames(Valuable::va_names_t&) const override
        { }

        bool eval(const Valuable::vars_cont_t&) override {
            return {};
        }
        void Eval(const Variable&, const Valuable&) override
        { }

        [[nodiscard]]
        Valuable::universal_lambda_t CompileIntoLambda(Valuable::variables_for_lambda_t) const noexcept override {
            return [](Valuable::universal_lambda_params_t) -> Valuable {
                return GlobalObject;
            };
        }

        Valuable& operator *=(const Valuable& v) override {
            if(v.IsProduct())
                return this->Become(v**this);
            else
                return this->Become(Product{*this,v});
        }

        Valuable& sq() override {
            return this->Become(Exponentiation{*this, 2});
        }

        std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override {
            std::pair<bool, Valuable> is;
            is.first = v.IsConstant();
            if (is.first) {
                is.first = this->OfSameType(v);
                if (is.first) {
                    is.second = this->Sq();
                }
            } else if (!(v.IsVa() || v.IsInt())) {
                is = v.IsMultiplicationSimplifiable(*this);
            }
            return is;
        }

        [[nodiscard]]
        bool operator==(const Valuable& v) const override {
            return this->OfSameType(v) || v.operator==(*this);
        }

        Valuable& operator/=(const Valuable& v) override {
            if (v == *this) {
                return this->Become(1);
            }
            return base::operator/=(v);
        }

        [[nodiscard]]
        const Valuable::vars_cont_t& getCommonVars() const override {
            return Valuable::emptyCommonVars();
        }

        [[nodiscard]]
        Valuable::vars_cont_t GetVaExps() const override { return {}; }

        [[nodiscard]]
        a_int Complexity() const noexcept override { return 1; }

        typename base::solutions_t Distinct() const override { return { *this }; }
        void Values(const std::function<bool(const Valuable&)>& f) const override { f(*this); }
        [[nodiscard]]
        Valuable Univariate() const override { return *this; }

        [[nodiscard]]
        Valuable InCommonWith(const Valuable& v) const override {
            return v.IsConstant() && this->OfSameType(v) ? v
                : ((v.IsSimple() || v.IsVa()) ? constants::one
                : v.InCommonWith(*this));
        }

    };

    namespace constants {
    class ConstNameAdder {
        static std::map<std::string_view, Valuable> SerializationNamesMap;

    public:
        APPLE_CONSTEXPR ConstNameAdder(const std::string_view& name, const Valuable& obj);
        static constexpr auto& GetConstantNamesMap() { return SerializationNamesMap; }
    };
    } // namespace constants

    template <class Chld>
    const/*init*/ Chld Constant<Chld>::GlobalObject;

    template <class Chld>
    const/*init*/ constants::ConstNameAdder
        Constant<Chld>::ConstNameToSerializationNamesMapAdder(Chld::SerializationName, Constant<Chld>::GlobalObject);

}} // namespace
