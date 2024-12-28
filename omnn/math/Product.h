//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once

#include <omnn/math/ValuableCollectionDescendantContract.h>
#include <omnn/math/Integer.h>
#include <omnn/math/Fraction.h>
#include <omnn/math/ProductOrderComparator.h>
#include <omnn/math/Variable.h>

#include <set>
#include <unordered_set>


namespace omnn{
namespace math {
    
    
    using product_container =
        //std::multiset<Valuable, ProductOrderComparator>
        //std::unordered_multiset<Valuable>
        std::set<Valuable, ProductOrderComparator>
        ;

class Product
    : public ValuableCollectionDescendantContract<Product, product_container>
{
    using base = ValuableCollectionDescendantContract<Product, product_container>;
    using base::cont;
    friend class Variable;
    cont members;
    max_exp_t vaExpsSum;

protected:
    void AddToVars(const Variable &item, const Valuable & exponentiation);
    void AddToVarsIfVaOrVaExp(const Valuable::vars_cont_t&);
    void AddToVarsIfVaOrVaExp(const Valuable &item);

public:
    
    using base::base;

    Product()
    : members{constants::one}
    {
        hash = constants::one.Hash();
    }
    Product(Product&&)=default;
    Product(const Product&)=default;
    Product(const std::initializer_list<Valuable>);
    
	constexpr const cont& GetConstCont() const override { return members; }
    iterator Had(iterator it) override;
    static bool VarSurdFactor(const Valuable&);

    // FIXME : waiting for virtual template methods to override as polymprphic method https://github.com/ohhmm/llvm-project/pull/1
    template <typename T> // override 
    const T* Divisor() const {
        auto it = GetFirstOccurence<T>();
        auto divisor = it != end() ? it->template As<T>() : nullptr;
        if (!divisor) {
            it = GetFirstOccurence<Product>();
            if (it != end()) {
                divisor = it->template as<Product>().template Divisor<T>();
            }
        }
        return divisor;
    }

    using base::Add;
    const iterator Add(Valuable&& item, const iterator hint) override;
    const iterator Add(const Valuable& item, const iterator hint) override;
    using base::Delete;
    void Delete(typename cont::iterator& it) override;
    void Update(iterator&, Valuable&&) override;
    void Update(iterator&, const Valuable&) override;

    const vars_cont_t& getCommonVars() const override;
    vars_cont_t getCommonVars(const vars_cont_t& with) const;
    
    Valuable getCommVal(const Product& with) const;
    Valuable InCommonWith(const Valuable& v) const override;
    max_exp_t findMaxVaExp();
    bool IsComesBefore(const Valuable& v) const override;
    Valuable calcFreeMember() const override;
    Valuable GCD(const Valuable&) const override;
    Valuable& gcd(const Product&);
    Valuable& gcd(const Valuable&) override;
    Valuable& operator+=(const Valuable&) override;
    std::pair<bool, Valuable> IsSummationSimplifiable(const Product&) const;
    std::pair<bool, Valuable> IsSummationSimplifiable(const Valuable&) const override;
    std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable&) const override;
    Valuable& operator*=(const Valuable& v) override;
    Valuable& operator /=(const Valuable& v) override;
    Valuable& operator %=(const Valuable& v) override;
    Valuable& operator ^=(const Valuable& v) override;
    bool operator ==(const Product& v) const;
    bool operator ==(const Valuable& v) const override;
    bool operator<(const Product& v) const;
    bool operator<(const Valuable& v) const override;
    Valuable Sign() const override;
    Valuable ToBool() const override;
    explicit operator double() const override;
    Valuable& d(const Variable& x) override;
    void optimize() override;
    Valuable Sqrt() const override;
    Valuable& sq() override;
    Valuable abs() const override;
    Valuable& reciprocal() override;
    const PrincipalSurd* PrincipalSurdFactor() const override;

    bool IsProduct() const override { return true; }
    bool IsZero() const override;
    size_t FillPolynomialCoefficients(std::vector<Valuable>& coefficients, const Variable& v) const override;
    std::pair<Valuable, Valuable> SplitSimplePart() const;
    std::pair<Valuable, Valuable> split_simple_part();
    Valuable LCMofMemberFractionDenominators() const override;
  
    Valuable operator()(const Variable& va) const override;
    Valuable operator()(const Variable&, const Valuable& augmentation) const override;
    void solve(const Variable& va, solutions_t& solutions) const override;
    Valuable::solutions_t Distinct() const override;
    
    std::ostream& code(std::ostream& out) const override;

    static constexpr auto GetBinaryOperationLambdaTemplate() {
        return [](const auto& _1st, const auto& _2nd) { return _1st * _2nd; };
    }

	vars_cont_t GetVaExps() const override;

protected:
    std::ostream& print(std::ostream& out) const override;
    cont& GetCont() override { return members; }
    Product(const vars_cont_t& v) : vars(v) { base::Add(constants::one); }
    
private:
    vars_cont_t vars;
};


}}
