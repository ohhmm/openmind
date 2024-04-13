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

#include "Integer.h"
#include "Variable.h"

namespace omnn {
namespace math {
    
    template<class T>
    class TypedVarHost;
    
    class VarHost
        : public std::enable_shared_from_this<VarHost>
    {
        using non_zero_log_t = std::map<Valuable::var_set_t, Valuable::solutions_t>;
        non_zero_log_t nonZeroItems;

    public:
        using ptr = std::shared_ptr<VarHost>;
        using cptr = std::shared_ptr<const VarHost>;
        using cref = const VarHost&;
        using hosted_storage_t = std::pair<Variable, std::string>;
        
        virtual ~VarHost() = default;

        template<class T>
        Variable New(const T& id) {
#ifndef NDEBUG
            auto host = dynamic_cast<TypedVarHost<T>*>(this);
            if (host != this) {
                LOG_AND_IMPLEMENT("wrong id type");
            }
#endif
                Variable v(sh());
                v.SetId(id);
                return v;
            }
        
		Variable New();

    protected:
        VarHost() = default;

        const any::any& GetId(const Variable&) const;

        Variable New(const any::any& id);

        virtual void AddNewId(const void* id) {
            IMPLEMENT;
        }
        virtual const hosted_storage_t& HostedStorage(const any::any& id) = 0;

    public:
        ptr sh() {
            return shared_from_this();
        }
        template<class T = int>
        static ptr make(){
            return ptr(static_cast<VarHost*>(new TypedVarHost<T>()));
        }
        template<class T = Valuable>
        static VarHost& Global(){
            static auto host = make<T>();
            return *host;
        }
        template<class T>
        static void inc(T&);
        virtual bool IsIntegerId() const = 0;
        virtual bool Has(const any::any& id) const = 0;
        virtual size_t Hash(const any::any& id) const = 0;
        virtual any::any NewVarId() = 0;
        virtual any::any CloneId(const any::any& a) = 0;
        virtual bool CompareIdsLess(const any::any& a, const any::any& b) const = 0;
        virtual bool CompareIdsEqual(const any::any& a, const any::any& b) const = 0;
        virtual std::string_view GetName(const any::any&) const {
            LOG_AND_IMPLEMENT("Implement TypedVarHost<>::GetName specialization");
            return {};
        }
        
        std::ostream& print(std::ostream& out, const any::any& v) const { return out << GetName(v); }

        virtual const Variable& Host(const any::any& id) = 0;
        virtual Integer Stored() const = 0;

        void LogNotZero(const Valuable&);
        bool TestRootConsistencyWithNonZeroLog(const Variable& variable, const Valuable& value) const;
        bool TestRootConsistencyWithNonZeroLog(const Valuable::vars_cont_t&) const;
    };
    
    /**
     * ensures variable id uniquness in a space of variables
     */
    template<class T>
    class TypedVarHost : public VarHost
    {
        std::set<T> varIds;
        std::map<T, hosted_storage_t> hosted;
        friend class VarHost;
        constexpr TypedVarHost()=default;
        
    protected:

        void AddNewId(const void* id) override {
            if (sizeof(void*) >= sizeof(T))
                varIds.insert(*reinterpret_cast<T*>(reinterpret_cast<void*>(&id)));
            else if (std::is_class<T>::value) {
                auto varId = static_cast<const T*>(id);
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

        const T& GetId(const Variable& va) const {
            auto& id = VarHost::GetId(va);
            auto idTp = any::any_cast<T>(&id);
            return *idTp;
        }
        
    public:
        
        any::any NewVarId() override {

            T t = {};
            const auto& last = varIds.size() ? *varIds.rbegin() : t;
            if constexpr (std::is_same<std::string, T>::value) {
                return TypedVarHost<T>::NewVarId();
            } else if constexpr (std::is_arithmetic<T>::value
                       || std::is_same<boost::multiprecision::cpp_int, T>::value
                       || std::is_same<Integer, T>::value
                       || std::is_same<Valuable, T>::value) {
                auto n = last;
                typename decltype(varIds)::iterator inserted;
                do {
                    inc(n);
                } while(varIds.find(n)!=varIds.end());
                varIds.insert(n);
                return n;
            } else {
                LOG_AND_IMPLEMENT("Implement new specialization for the variable object template<> any::any TypedVarHost<T>::NewVarId()!");
            }
            
        }
        
        any::any CloneId(const any::any& a) override {
            return a;
        }
        
        bool IsIntegerId() const override {
            return std::is_integral<T>::value || std::is_same<T, Integer>::value;
        }

        bool Has(const any::any& id) const override {
            IMPLEMENT
            return varIds.find(any::any_cast<T>(id)) != varIds.end();
        }
        
        size_t Hash(const any::any& id) const override {
            return std::hash<T>()(any::any_cast<T>(id));
        }
        
        bool CompareIdsLess(const any::any& a, const any::any& b) const override {
            return any::any_cast<T>(a) < any::any_cast<T>(b);
        }
        
        bool CompareIdsEqual(const any::any& a, const any::any& b) const override {
            auto& ca = any::any_cast<const T&>(a);
            auto& cb = any::any_cast<const T&>(b);
            return ca == cb;
        }
        
        std::string_view GetName(const any::any& v) const override {
            LOG_AND_IMPLEMENT("Implement TypedVarHost<" << typeid(T).name() << ">::GetName specialization");
            return {};
        }
        
        hosted_storage_t& HostedStorage(const any::any& id) override {
            using namespace std::string_literals;

            const T* idTp = any::any_cast<T>(&id);

            auto it = hosted.end();
            if constexpr (std::is_same<T, std::string>::value) {
                if (!idTp) { // try other string types
                    const std::string_view* sv = any::any_cast<std::string_view>(&id);
                    if (sv) {
                        T id(*sv);
                        it = hosted.find(id);
                        if (it == hosted.end()) {
                            it = hosted.emplace(id, hosted_storage_t{New(id), ""s}).first;
                        }
                    } else {
						IMPLEMENT
					}
                }
            }
            if (it == hosted.end()) {
                const T& idT = *idTp;
                it = hosted.find(idT);
                if (it == hosted.end()) {
                    it = hosted.emplace(idT, hosted_storage_t{New(id), ""s}).first;
                }
            }

            return it->second;
        }

        const Variable& Host(const any::any& id) override {
            return HostedStorage(id).first;
        }

        Integer Stored() const override {
            return a_int(hosted.size());
        }
    };

    template<>
    void VarHost::inc<>(std::string&);

    template<class T>
    void VarHost::inc(T& t)
    {
        ++t;
    }

    template<>
    any::any TypedVarHost<std::string>::NewVarId();

    template<>
	std::string_view TypedVarHost<std::string>::GetName(const any::any& v) const;

    template<>
	std::string_view TypedVarHost<Valuable>::GetName(const any::any& v) const;

} /* namespace math */
} /* namespace omnn */
