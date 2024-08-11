/*
 * VarHost.h
 *
 *  Created on: 25 авг. 2017 г.
 *      Author: sergejkrivonos
 */

#pragma once

//#include <any>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>

#include <boost/any.hpp>

#include <omnn/math/Integer.h>
#include <omnn/math/Variable.h>

namespace omnn {
namespace math {

    template<class T>
    class TypedVarHost;

    class VarHost
        : public std::enable_shared_from_this<VarHost>
    {
        using base = std::enable_shared_from_this<VarHost>;
        using non_zero_log_t = std::map<Valuable::var_set_t, Valuable::solutions_t>; // or "known that not" equations
        non_zero_log_t nonZeroItems;
        static thread_local bool add_non_zero_mode_on;

    public:
        using ptr = std::shared_ptr<VarHost>;
        using cptr = std::shared_ptr<const VarHost>;
        using cref = const VarHost&;
        using hosted_storage_t = std::pair<Variable, std::string>;

        class [[maybe_unused]] NonZeroLogOffScope
        {
            bool opts;
        public:
            MSVC_CONSTEXPR
            NonZeroLogOffScope() : opts(add_non_zero_mode_on) { add_non_zero_mode_on = {}; }
            MSVC_CONSTEXPR
            ~NonZeroLogOffScope() { add_non_zero_mode_on = opts; }
        };

        virtual ~VarHost() = default;

        template<class T>
        Variable New(const T& id) {
#ifndef NDEBUG
            auto host = dynamic_cast<TypedVarHost<T>*>(this);
            if (host != this) {
                LOG_AND_IMPLEMENT("wrong id type");
            }
#endif
            Variable v(shared_from_this());
            v.SetId(id);
            return v;
        }

		Variable New();

    protected:
        VarHost() = default;

        const ::std::any& GetId(const Variable&) const;

        Variable New(const ::std::any& id);

        virtual void AddNewId(const void* id) {
            LOG_AND_IMPLEMENT("Implement TypedVarHost<>::AddNewId specialization")
        }
        virtual const hosted_storage_t& HostedStorage(const ::std::any& id) = 0;

    public:
        using base::shared_from_this;

        template<class T = int>
        static ptr make(){
            return static_pointer_cast<VarHost>(std::make_shared<TypedVarHost<T>>());
        }

        template<class T = Valuable>
        static VarHost& Global(){
            return TypedVarHost<T>::Global();
        }

        template<class T>
        static void inc(T&);
        virtual bool IsIntegerId() const = 0;
        virtual bool Has(const ::std::any& id) const = 0;
        virtual size_t Hash(const ::std::any& id) const = 0;
        virtual ::std::any NewVarId() = 0;
        virtual ::std::any CloneId(const ::std::any& a) = 0;
        virtual bool CompareIdsLess(const ::std::any& a, const ::std::any& b) const = 0;
        virtual bool CompareIdsEqual(const ::std::any& a, const ::std::any& b) const = 0;
        virtual std::string_view GetName(const ::std::any&) const {
            LOG_AND_IMPLEMENT("Implement TypedVarHost<>::GetName specialization");
            return {};
        }

        std::ostream& print(std::ostream& out, const ::std::any& v) const { return out << GetName(v); }

        virtual const Variable& Host(const ::std::any& id) = 0;
        virtual Integer Stored() const = 0;

        void LogNotZero(const Valuable&);
        void LogNotZeroBypassScopes(const Valuable&);
        bool TestRootConsistencyWithNonZeroLog(const Variable& variable, const Valuable& value) const;
        bool TestRootConsistencyWithNonZeroLog(const Valuable::vars_cont_t&) const;
    };

    /**
     * ensures variable id uniquness in a space of variables
     */
    template<class VarIdT>
    class TypedVarHost : public VarHost
    {
        using self_t = TypedVarHost<VarIdT>;

        std::set<VarIdT> varIds;
        std::map<VarIdT, hosted_storage_t> hosted;
        friend class VarHost;

    protected:
        static ptr host;
        static VarHost& Global() {
            if (!host) {
                host = make<VarIdT>();
            }
            return *host;
        }

        void AddNewId(const void* id) override {
            if (sizeof(void*) >= sizeof(VarIdT))
                varIds.insert(*reinterpret_cast<VarIdT*>(reinterpret_cast<void*>(&id)));
            else if (std::is_class<VarIdT>::value) {
                auto varId = static_cast<const VarIdT*>(id);
                if (varId)
                {
                    varIds.insert(*varId);
                }
                else
                {
                    throw "Wrong param";
                }
            } else {
                IMPLEMENT;
            }
        }

        const VarIdT& GetId(const Variable& va) const {
            auto& id = VarHost::GetId(va);
            auto idTp = ::std::any_cast<VarIdT>(&id);
            return *idTp;
        }

    public:
        constexpr TypedVarHost() = default;

        static constexpr bool IsArithmeticId = std::is_integral<VarIdT>::value
            || std::is_arithmetic<VarIdT>::value
            || std::is_same<VarIdT, Valuable>::value
            || std::is_same<VarIdT, Integer>::value
            || std::is_same<boost::multiprecision::cpp_int, VarIdT>::value;

        ~TypedVarHost() override = default;

        ::std::any NewVarId() override {

            VarIdT t = {};
            const auto& last = varIds.size() ? *varIds.rbegin() : t;
            if constexpr (std::is_same<std::string, VarIdT>::value) {
                return self_t::NewVarId();
            } else if constexpr (IsArithmeticId) {
                auto n = last;
                typename decltype(varIds)::iterator inserted;
                do {
                    inc(n);
                } while(varIds.find(n)!=varIds.end());
                varIds.insert(n);
                return n;
            } else {
                LOG_AND_IMPLEMENT("Implement new specialization for the variable object template<> ::std::any TypedVarHost<T>::NewVarId()!");
            }

        }

        ::std::any CloneId(const ::std::any& a) override {
            return a;
        }

        bool IsIntegerId() const override {
            return IsArithmeticId;
        }

        bool Has(const ::std::any& id) const override {
            bool hasId = {};
            using namespace std::string_literals;

            const VarIdT* idTp = ::std::any_cast<VarIdT>(&id);

            auto it = hosted.end();
            if constexpr (std::is_same<VarIdT, std::string>::value) {
                if (!idTp) { // try other string types
                    const std::string_view* sv = ::std::any_cast<std::string_view>(&id);
                    if (sv) {
                        VarIdT id(*sv);
                        it = hosted.find(id);
                        hasId = it != hosted.end();
                    } else {
                        LOG_AND_IMPLEMENT("Function VarHost::Has not implemented for std::string specialization.");
                    }
                }
            }
            else if (it == hosted.end()) {
                const VarIdT& idT = *idTp;
                it = hosted.find(idT);
                hasId = it != hosted.end();
            }

            return hasId;
        }

        size_t Hash(const ::std::any& id) const override {
            return std::hash<VarIdT>()(::std::any_cast<VarIdT>(id));
        }

        bool CompareIdsLess(const ::std::any& a, const ::std::any& b) const override {
            return ::std::any_cast<VarIdT>(a) < ::std::any_cast<VarIdT>(b);
        }

        bool CompareIdsEqual(const ::std::any& a, const ::std::any& b) const override {
            auto& ca = ::std::any_cast<const VarIdT&>(a);
            auto& cb = ::std::any_cast<const VarIdT&>(b);
            return ca == cb;
        }

        std::string_view GetName(const ::std::any& v) const override {
            LOG_AND_IMPLEMENT("Implement TypedVarHost<" << typeid(VarIdT).name() << ">::GetName specialization");
            return {};
        }

        hosted_storage_t& HostedStorage(const ::std::any& id) override {
            using namespace std::string_literals;

            const VarIdT* idTp = ::std::any_cast<VarIdT>(&id);

            auto it = hosted.end();
            if constexpr (std::is_same<VarIdT, std::string>::value) {
                if (!idTp) { // try other string types
                    const std::string_view* sv = ::std::any_cast<std::string_view>(&id);
                    if (sv) {
                        VarIdT id(*sv);
                        it = hosted.find(id);
                        if (it == hosted.end()) {
                            it = hosted.emplace(id, hosted_storage_t{New(id), ""s}).first;
                        }
                    } else {
                        LOG_AND_IMPLEMENT("Function HostedStorage not implemented for std::string specialization.");
                    }
                }
            }
            if (it == hosted.end()) {
                const VarIdT& idT = *idTp;
                it = hosted.find(idT);
                if (it == hosted.end()) {
                    it = hosted.emplace(idT, hosted_storage_t{New(id), ""s}).first;
                }
            }

            return it->second;
        }

        const Variable& Host(const ::std::any& id) override {
            return HostedStorage(id).first;
        }

        Integer Stored() const override {
            return a_int(hosted.size());
        }
    };

    template<class T>
    VarHost::ptr TypedVarHost<T>::host = {};

    template<>
    void VarHost::inc<>(std::string&);

    template<class T>
    void VarHost::inc(T& t)
    {
        ++t;
    }

    template<>
    ::std::any TypedVarHost<std::string>::NewVarId();

    template<>
	std::string_view TypedVarHost<std::string>::GetName(const ::std::any& v) const;

    template<>
	std::string_view TypedVarHost<Valuable>::GetName(const ::std::any& v) const;

} /* namespace math */
} /* namespace omnn */
