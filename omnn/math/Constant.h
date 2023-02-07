//
// Created by Сергей Кривонос on 01.09.17.
//
#pragma once
#include "Exponentiation.h"
#include "Product.h"

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
        static const /*init*/ Chld GlobalObject;

        bool IsConstant() const override { return true; }
        bool IsSimple() const override { return true; }
        Valuable::YesNoMaybe IsMultival() const override { return Valuable::YesNoMaybe::No; }

        const Variable* FindVa() const override {
            return {};
        }
        
        bool HasVa(const Variable& va) const override {
            return {};
        }
        
        void CollectVa(std::set<Variable>& s) const override
        { }
        void CollectVaNames(Valuable::va_names_t& s) const override
        { }
        
        void Eval(const Variable& va, const Valuable& v) override
        { }
        
        Valuable& operator *=(const Valuable& v) override {
            if(v.IsProduct())
                return this->Become(v**this);
            else
                return this->Become(Product{*this,v});
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
        
        bool operator==(const Valuable& v) const override {
            return this->OfSameType(v);
        }
        
        Valuable& operator/=(const Valuable& v) override {
            if (v == *this)
                return this->Become(1);
            else
                return base::operator/=(v);
        }

        const Valuable::vars_cont_t& getCommonVars() const override {
            return Valuable::emptyCommonVars();
        }
        
        a_int Complexity() const override { return 1; }

        typename base::solutions_t Distinct() const override { return { *this }; }
        void Values(const std::function<bool(const Valuable&)>& f) const override { f(*this); }
        Valuable Univariate() const override { return *this; }

        Valuable InCommonWith(const Valuable& v) const override {
            return v.IsConstant() && this->OfSameType(v) ? v
				: (v.IsSimple() ? 1_v
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

