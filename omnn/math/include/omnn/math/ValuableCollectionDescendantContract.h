//
// Created by Сергей Кривонос on 01.09.17.
//
#pragma once
#include <algorithm>
#include <atomic>
#include <execution>
#include <future>
#include <iterator>
#include <ranges>
#include <thread>

#include "omnn/math/ValuableDescendantContract.h"
#include "omnn/math/CollectionForward.h"
#include "omnn/math/OptimizedCollection.h"
#include <omnn/rt/each.hpp>
//#include <omnn/rt/iterator_transforming_wrapper.hpp>
#include "omnn/math/Exponentiation.h"
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

namespace omnn::math {

namespace lambda_types {
    using lambda_t = Valuable::universal_lambda_t;
    using params_t = Valuable::universal_lambda_params_t;
    using binary_op_t = std::function<Valuable(const Valuable&, const Valuable&)>;
    using reduce_op_t = std::function<lambda_t(const lambda_t&, const lambda_t&)>;
    using initial_lambda_t = std::function<Valuable(params_t)>;
}

template <class ChildT, class ContT>
class ValuableCollectionDescendantContract : public ValuableDescendantContract<ChildT>
{
    using base = ValuableDescendantContract<ChildT>;

protected:
    using cont = ContT;
    using collection_type = cont;
    virtual collection_type& GetCollection() = 0;
    virtual const collection_type& GetCollection() const = 0;

    // Use namespace-scoped lambda types
    using lambda_t = lambda_types::lambda_t;
    using params_t = lambda_types::params_t;
    using binary_op_t = lambda_types::binary_op_t;
    using reduce_op_t = lambda_types::reduce_op_t;
    using initial_lambda_t = lambda_types::initial_lambda_t;

    // Lambda creation methods
    binary_op_t GetBinaryOperationLambda() const {
        const auto* derived = static_cast<const ChildT*>(this);
        return [derived](const Valuable& a, const Valuable& b) -> Valuable {
            return (derived->GetBinaryOperationLambdaTemplate())(a, b);
        };
    }

    reduce_op_t GetReductionLambda() const {
        const auto* derived = static_cast<const ChildT*>(this);
        return [derived](const lambda_t& a, const lambda_t& b) -> lambda_t {
            return [derived, a, b](params_t params) -> Valuable {
                return (derived->GetBinaryOperationLambdaTemplate())(
                    a(params), b(params)
                );
            };
        };
    }

    lambda_t CreateInitialLambda() const {
        return [](params_t params) -> Valuable {
            return Valuable(ChildT());
        };
    }

public:
    // Use ContT's iterator types directly
    using iterator = typename ContT::iterator;
    using const_iterator = typename ContT::const_iterator;
    using const_reference = typename ContT::const_reference;
    using value_type = typename ContT::value_type;

    ValuableCollectionDescendantContract() = default;
    ValuableCollectionDescendantContract(ValuableCollectionDescendantContract&&)=default;
    ValuableCollectionDescendantContract(const ValuableCollectionDescendantContract&)=default;
    ValuableCollectionDescendantContract& operator=(ValuableCollectionDescendantContract&&)=default;
    ValuableCollectionDescendantContract& operator=(const ValuableCollectionDescendantContract&)=default;

    // Base class methods for collection operations
    virtual const iterator Add(const Valuable& item, const iterator hint) {
        Valuable::hash ^= item.Hash();
        auto& c = GetCollection();
        this->optimized = {};
        return hint == c.end() ? c.insert(item).first : c.insert(hint, item);
    }

    virtual const iterator Add(Valuable&& item, const iterator hint) {
        this->optimized = {};
        if (&item.get() == &item) {
            (void) item.SharedFromThis();
        }
        Valuable::hash ^= item.Hash();
        auto& c = GetCollection();
        return hint == c.end() ? c.insert(std::move(item)).first : c.insert(hint, std::move(item));
    }

    virtual void Delete(iterator& it) {
        Valuable::hash ^= it->Hash();
        auto& c = GetCollection();
        c.erase(it++);
        this->optimized = {};
    }

    constexpr auto begin() noexcept { return GetCollection().begin(); }
    constexpr auto end() noexcept { return GetCollection().end(); }
    constexpr auto begin() const noexcept { return GetCollection().begin(); }
    constexpr auto end() const noexcept { return GetCollection().end(); }
    constexpr auto cbegin() const noexcept { return GetCollection().cbegin(); }
    constexpr auto cend() const noexcept { return GetCollection().cend(); }
    constexpr auto rbegin() noexcept { return GetCollection().rbegin(); }
    constexpr auto rend() noexcept { return GetCollection().rend(); }
    constexpr auto crbegin() const noexcept { return GetCollection().crbegin(); }
    constexpr auto crend() const noexcept { return GetCollection().crend(); }

    size_t size() const
    {
        return GetConstCont().size();
    }

    static iterator getit(iterator it){
        return it;
    }
    iterator getit(const std::pair<iterator,bool>& it){
        auto inserted = it.second;
        if(!inserted){
            return Had(it.first);
        }
        return it.first;
    }
    virtual iterator Had(iterator it)
    {
        return it;  // Default implementation
    }

    bool VarSurdFactor(const iterator it) const {
        return VarSurdFactor(*it);
    }

    bool HasSurdFactor() const {
        return std::any_of(begin(), end(), ChildT::VarSurdFactor);
    }

    template <class ItemT>
    const iterator Add(ItemT&& item)
    {
        return this->Add(std::forward<ItemT>(item), end());
    }

    template<class T>
    auto GetFirstOccurence() const
    {
        auto& c = GetConstCont();
        auto i = c.begin();
        for(auto e = c.end(); i != e; ++i)
            if(i->template Is<T>())
                break;
        return i;
    }

    template <class T>
    auto GetFirstOccurence() {
        auto& c = GetCont();
        auto i = c.begin();
        for (auto e = c.end(); i != e; ++i)
            if (i->template Is<T>())
                break;
        return i;
    }

    a_int Complexity() const override {
        a_int c = 0;
        for(auto& m : GetConstCont())
            c += m.Complexity();
        return c;
    }

    bool HasValueType(const std::type_info& type) const
    {
        for(const auto& a : GetConstCont())
            if(typeid(a.get()) == type)
                return true;
        return false;
    }

    template<class T>
    bool HasValueType() const
    {
        auto& cc = GetConstCont();
        return std::any_of(
            cc.begin(), cc.end(),
            [](auto& m) {
                return m.template Is<T>();
            });
    }

    bool Has(const Valuable& v) const
    {
        auto& c = GetConstCont();
        // c.find uses order comparator
        auto found = std::find(c.begin(), c.end(), v); // using simple equality finder std::find instead
        auto foundSome = found != c.cend();
        auto has = foundSome && found->Same(v);
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
        if(foundSome && !has && this->is_optimized() && v.is_optimized()){
            LOG_AND_IMPLEMENT("Bug in comparator or object type differs: " << this->str() << " has " << v);
        }
#endif
        return has;
    }

    const Variable* FindVa() const override
    {
        for (auto& i : GetConstCont())
        {
            auto va = i.FindVa();
            if (va)
                return va;
        }
        return nullptr;
    }

    bool HasVa(const Variable& va) const override
    {
        for (auto& i : GetConstCont())
            if(i.HasVa(va))
                return true;
        return false;
    }

    void CollectVa(std::set<Variable>& s) const override {
        for (auto& i : GetConstCont())
            i.CollectVa(s);
    }
    void CollectVaNames(Valuable::va_names_t& s) const override {
        for (auto& i : GetConstCont())
            i.CollectVaNames(s);
    }

    void CallForEach(const std::function<void (const Valuable&)>& call) const {
        rt::each(GetConstCont(), call);
    }

    Valuable Each(const std::function<Valuable(const Valuable&)>& m) const {
        ChildT c;
        for (auto& i : GetConstCont())
            c.Add(m(i));
        return c;
    }

    bool IsSimple() const override {
        return std::all_of(begin(), end(), [](auto& m){return m.IsSimple();});
    }

    bool IsPolynomial(const Variable& v) const override {
        return std::all_of(begin(), end(), [&](auto& m) { return m.IsPolynomial(v); });
    }

    Valuable::YesNoMaybe IsMultival() const override {
        Valuable::YesNoMaybe is;
        if(size()==0) {
            is = Valuable::YesNoMaybe::No;
        } else {
        auto it = begin();
            is = it->IsMultival();
        if(size()!=1)
            while(++it != end())
                is = is || it->IsMultival();
        }
        return is;
    }

    void Values(const std::function<bool(const Valuable&)>& fun) const override {
        auto sharedValuesProjection = std::vector<std::vector<Valuable>>();
        for (auto& m : GetConstCont()) {
            if (m.IsMultival() == Valuable::YesNoMaybe::Yes) {
                if (sharedValuesProjection.size()) {
                    auto sharedValuesProjectionCopy = std::move(sharedValuesProjection);
                    m.Values([&](const Valuable& value) {
                        decltype(sharedValuesProjection) copy = sharedValuesProjectionCopy;
                        for (auto& copyMember : copy) {
                            copyMember.emplace_back(value);
                        }
                        std::move(copy.begin(), copy.end(), std::back_inserter(sharedValuesProjection));
                        return true;
                    });
                } else {
                    m.Values([&](const Valuable& value) {
                        sharedValuesProjection.emplace_back().emplace_back(value.Link());
                        return true;
                    });
                }
            } else {
                if (sharedValuesProjection.size()) {
                    for (auto& projection : sharedValuesProjection) {
                        projection.emplace_back(m.Link());
                    }
                } else {
                    sharedValuesProjection.emplace_back().emplace_back(m.Link());
                }
            }
        }

        for (auto&& projection : sharedValuesProjection) {
            ChildT value;
            for (auto& item : projection) {
                value.Add(item);
            }
            fun(value);
        }
    }

    bool eval(const std::map<Variable, Valuable>& with) override {
        bool evaluated = {};
        auto& members = GetCollection();
        Valuable::SetView(Valuable::View::Calc);
        SmallVector<Valuable, 16> evaluatedMembers;
        auto e = members.end();

        // Use hardware-aware threshold for async processing
        if (members.size() < get_async_threshold()) {
            for (iterator it = members.begin(); it != e; ) {
                auto& v = const_cast<Valuable&>(*it);
                if (v.eval(with)) {
                    evaluated = true;
                    // Optimize Extract/Add cycle by moving directly into SmallVector
                    evaluatedMembers.push_back(Extract(it++));
                } else
                    ++it;
            }
        } else {
            rt::StoringTasksQueue<iterator> jobs({});
            std::atomic<int> ec{0};
            jobs.AddIteratorTasks(members, [&](auto it) {
                auto& v = const_cast<Valuable&>(*it);
                if (v.eval(with)) {
                    ec.fetch_add(1, std::memory_order_relaxed);
                } else {
                    it = e;
                }
                return it;
            });

            evaluated = ec.load(std::memory_order_relaxed) != 0 || evaluated;

            // Batch process results to reduce synchronization overhead
            if (evaluated) {
                auto results = jobs.GetAllResults();
                for (auto it : results) {
                    if (it != e) {
                        evaluatedMembers.push_back(Extract(it));
                    }
                }
            }
        }

        if (evaluated) {
            // Optimize by moving elements directly
            for (auto&& val : evaluatedMembers) {
                this->Add(std::move(val));
            }
            Valuable::optimized = {};
            if (Complexity() <= 8) {
                Valuable::OptimizeOn on;
                this->optimize();
            }
        }

        return evaluated;
    }

    void Eval(const Variable& va, const Valuable& v) override
    {
        Valuable::SetView(Valuable::View::Calc);
        auto& c = GetCollection();
        auto e = c.end();
        bool updated;
        do
        {
            updated = {};
            for(auto i = c.begin(); i != e;)
            {
                if(i->HasVa(va)){
                    auto co = *i;
                    co.Eval(va,v);
                    co.optimize();
                    Update(i,co);
                    updated = true;
                } else {
                    ++i;
                }
            }
        } while (updated);

//            if(!FindVa())
//                this->optimize();
    }

    virtual void Update(iterator& it, Valuable&& v)
    {
        Valuable moved;
        moved = std::move(v);
        this->Delete(it); // original v may be [sub]object of *it
        it = this->Add(std::move(moved), it);
        Valuable::optimized = {};
    }

    virtual void Update(iterator& it, const Valuable& v)
    {
        auto copy = v;
        this->Delete(it);
        it = this->Add(std::move(copy), it);
        Valuable::optimized = {};
    }

    virtual Valuable Extract(const iterator it)
    {
        Valuable::hash ^= it->Hash();
        auto findNewMaxVaExp = it->getMaxVaExp() == this->getMaxVaExp();
        auto& c = GetCont();
        Valuable::optimized &= c.size() > 2;
        auto extracted = std::move(c.extract(it).value());
        if (findNewMaxVaExp)
            Valuable::maxVaExp = this->Ptr()->findMaxVaExp();
        return extracted;
    }

    Valuable Extract()
    {
        return Extract(begin());
    }

    virtual void Update(iterator& it) {
        auto e = this->Extract(it++);
        it = Add(e, it);
    }

    [[nodiscard]]
    Valuable::universal_lambda_t CompileIntoLambda(Valuable::variables_for_lambda_t vars) const override {
        const auto& cont = GetConstCont();
        std::vector<lambda_t> lambdas;
        lambdas.reserve(cont.size());

        for (auto it = cont.begin(); it != cont.end(); ++it) {
            lambdas.push_back(it->CompileIntoLambda(vars));
        }

        const auto* derived = static_cast<const ChildT*>(this);
        auto binary_op = derived->GetBinaryOperationLambda();

        auto reduce_op = [binary_op](const lambda_t& l1, const lambda_t& l2) -> lambda_t {
            return [binary_op, l1, l2](params_t params) -> Valuable {
                return binary_op(l1(params), l2(params));
            };
        };

        const auto collection_size = cont.size();
        if (collection_size < 2) {
            return lambdas.empty() ? CreateInitialLambda() : lambdas[0];
        }

        return std::reduce(
            collection_size < 1000 ? std::execution::seq : std::execution::par,
            lambdas.begin(),
            lambdas.end(),
            CreateInitialLambda(),
            reduce_op
        );
    }

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & archive, const unsigned int version) {
        archive & boost::serialization::base_object<base>(*this);
        archive & GetCollection();
    }
};

}
