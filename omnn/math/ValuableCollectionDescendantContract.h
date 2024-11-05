//
// Created by Сергей Кривонос on 01.09.17.
//
#pragma once
#include "Exponentiation.h"
#include <algorithm>
#include <future>
#include <iterator>
#include <ranges>

#include <omnn/rt/each.hpp>
//#include <omnn/rt/iterator_transforming_wrapper.hpp>

namespace omnn::math {

    template <class ChildT, class ContT>
    class ValuableCollectionDescendantContract : public ValuableDescendantContract<ChildT>
    {
        using base = ValuableDescendantContract<ChildT>;

    protected:
        using cont = ContT;
        virtual cont& GetCont() = 0;
        [[nodiscard]]
        bool IsSubObject(const Valuable& o) const override {
            auto is = this == &o.get();
            if (!is) {
                for(auto& m : GetConstCont()) {
                    is = &m.get() == &o.get();
                    if(is)
                        break;
                }
            }
            return is;
        }

    public:
        using iterator = typename cont::iterator;
        using const_reference = typename ContT::const_reference;

        using base::base;
        ValuableCollectionDescendantContract(ValuableCollectionDescendantContract&&)=default;
        ValuableCollectionDescendantContract(const ValuableCollectionDescendantContract&)=default;
        ValuableCollectionDescendantContract& operator=(ValuableCollectionDescendantContract&&)=default;
        ValuableCollectionDescendantContract& operator=(const ValuableCollectionDescendantContract&)=default;

        virtual const cont& GetConstCont() const = 0;

        constexpr auto begin() noexcept { return GetCont().begin(); }
        constexpr auto end() noexcept { return GetCont().end(); }
        constexpr auto begin() const noexcept { return GetConstCont().begin(); }
        constexpr auto end() const noexcept { return GetConstCont().end(); }
        constexpr auto cbegin() const noexcept { return GetConstCont().cbegin(); }
        constexpr auto cend() const noexcept { return GetConstCont().cend(); }
        constexpr auto rbegin() noexcept { return GetCont().rbegin(); }
        constexpr auto rend() noexcept { return GetCont().rend(); }
        constexpr auto crbegin() const noexcept { return GetConstCont().crbegin(); }
        constexpr auto crend() const noexcept { return GetConstCont().crend(); }

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
            IMPLEMENT
        }

        bool VarSurdFactor(const iterator it) const {
            return VarSurdFactor(*it);
        }

        bool HasSurdFactor() const {
            return std::any_of(begin(), end(), ChildT::VarSurdFactor);
		}

        virtual const iterator Add(const Valuable& item, const iterator hint)
        {
            Valuable::hash ^= item.Hash();
            auto& c = GetCont();
            this->optimized = {};
            auto it = hint == c.end() ? getit(c.emplace(item)) : getit(c.insert(hint, item));
            return it;
        }

        virtual const iterator Add(Valuable&& item, const iterator hint)
        {
            this->optimized = {};
            if (&item.get() == &item) {
                (void) item.SharedFromThis();
            }
            Valuable::hash ^= item.Hash();
            auto& c = GetCont();
            auto it = hint == c.end()
                ? getit(c.emplace(std::move(item)))
                : getit(c.insert(hint, std::move(item)));
            return it;
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
            auto& members = GetCont();
            Valuable::SetView(Valuable::View::Calc);
            ChildT c;
            if (members.size() < 100) {
                for(auto&& cm:GetCont()){
                    auto&& m = const_cast<Valuable&&>(cm);
                    evaluated = m.eval(with) || evaluated;
                    c.Add(std::move(m));
                }
            } else {
                std::deque<std::future<bool>> jobs;
                for(auto& m : members) {
                    jobs.push_back(std::async(std::launch::async, [&]() {
                        return const_cast<Valuable&>(m).eval(with);
                    }));
                }
                for(auto&& m : members) {
                    auto& f = jobs.front();
                    evaluated = f.get() || evaluated;
                    c.Add(std::move(const_cast<Valuable&&>(m)));
                    jobs.pop_front();
                }
            }
            Valuable::Become(std::move(c));
            if( evaluated && Complexity() <=8 ){
                Valuable::OptimizeOn on;
                this->optimize();
            }
            return evaluated;
        }

        void Eval(const Variable& va, const Valuable& v) override
        {
            Valuable::SetView(Valuable::View::Calc);
            auto& c = GetCont();
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

        virtual void Delete(iterator& it) {
            Valuable::hash ^= it->Hash();
            auto& c = GetCont();
            auto findNewMaxVaExp = it->getMaxVaExp() == this->getMaxVaExp();
            c.erase(it++);
            Valuable::optimized &= c.size() > 1;
            if (findNewMaxVaExp)
                Valuable::maxVaExp = this->Ptr()->findMaxVaExp(); // TODO: consider heap structure
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
            auto range = GetConstCont()
                | std::ranges::views::transform([&](auto& value){return value.CompileIntoLambda(vars);});
            static Valuable::universal_lambda_t getInitialValue = [](Valuable::universal_lambda_params_t) {
                return Valuable(ChildT());
            };
            return std::reduce(PAR
                range.begin(), range.end(), getInitialValue,
                [&]<typename LambdaFwd1, typename LambdaFwd2>(LambdaFwd1&& item1, LambdaFwd2&& item2) {
                    return [
                        lambda1 = std::forward<LambdaFwd1>(item1),
                        lambda2 = std::forward<LambdaFwd2>(item2)
                    ]
                    (auto params) -> Valuable
                    {
                        return ChildT::GetBinaryOperationLambdaTemplate()(lambda1(params), lambda2(params));
                    };
                });
        }

    private:
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive & archive, const unsigned int version) {
            archive & boost::serialization::base_object<base>(*this);
            archive & GetCont();
        }
    };
}
