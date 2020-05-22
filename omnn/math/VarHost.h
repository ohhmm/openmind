/*
 * VarHost.h
 *
 *  Created on: 25 авг. 2017 г.
 *      Author: sergejkrivonos
 */

#pragma once
//#include <any>
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

    namespace any=boost;
    
    template<class T>
    class TypedVarHost;
    
    class VarHost
        : public std::enable_shared_from_this<VarHost>
    {
    public:
        using ptr = std::shared_ptr<VarHost>;
        using cref = const VarHost&;
        virtual ~VarHost() = default;

        template<class T>
        Variable New(const T& id) {
            auto host = dynamic_cast<TypedVarHost<T>*>(this);
            if (host) {
                AddNewId(sizeof(void*) >= sizeof(T)
                         ? *(void**)(&id)
                         : (void*)(new T(id)));
                Variable v(sh());
                v.SetId(id);
                return v;
            }
            else
            {
                throw "wrong id type";
            }
        }
        
        Variable New()
        {
            Variable v(sh());
            v.SetId(NewVarId());
            return v;
        }

    protected:
        VarHost() = default;
        virtual void AddNewId(const void* id) {
            throw "Implement!";
        }
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
        virtual bool Has(const any::any& id) const = 0;
        virtual size_t Hash(const any::any& id) const = 0;
        virtual any::any NewVarId() = 0;
        virtual any::any CloneId(const any::any& a) = 0;
        virtual bool CompareIdsLess(const any::any& a, const any::any& b) const = 0;
        virtual bool CompareIdsEqual(const any::any& a, const any::any& b) const = 0;
        virtual std::ostream& print(std::ostream& out, const any::any& v) const = 0;
        virtual const Variable& Host(const any::any& id) = 0;
    };
    
    /**
     * ensures variable id uniquness in a space of variables
     */
    template<class T>
    class TypedVarHost : public VarHost
    {
        std::set<T> varIds;
        std::map<T, Variable> hosted;
        friend class VarHost;
        TypedVarHost()=default;
        
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
                throw "Implement!";
            }
        }
        
    public:
        
        any::any NewVarId() override {

            T t = {};
            const auto& last = varIds.size() ? *varIds.rbegin() : t;
            if (std::is_same<std::string, T>::value) {
                return TypedVarHost<T>::NewVarId();
            } else if (std::is_arithmetic<T>::value
                       || std::is_same<boost::multiprecision::cpp_int, T>::value
                       || std::is_same<Integer, T>::value
                       || std::is_same<Valuable, T>::value) {
                auto n = last;
                typename decltype(varIds)::iterator inserted;
                inc(n);
                while(varIds.find(n)!=varIds.end());
                varIds.insert(n);
                return n;
            } else {
                throw "Implement new specialization for the variable object template<> any::any TypedVarHost<T>::NewVarId()!";
            }
            
        }
        
        any::any CloneId(const any::any& a) override {
            return any::any_cast<T>(a);
        }
        
        bool Has(const any::any& id) const override {
            return varIds.find(any::any_cast<T>(id)) != varIds.end();
        }
        
        size_t Hash(const any::any& id) const override {
            return std::hash<T>()(any::any_cast<T>(id));
        }
        
        bool CompareIdsLess(const any::any& a, const any::any& b) const override {
            return any::any_cast<T>(a) < any::any_cast<T>(b);
        }
        
        bool CompareIdsEqual(const any::any& a, const any::any& b) const override {
            auto ca = any::any_cast<const T&>(a);
            auto cb = any::any_cast<const T&>(b);
            bool eq = ca == cb;
            return eq;
        }
        
        std::ostream& print(std::ostream& out, const any::any& v) const override {
            auto c = any::any_cast<T>(v);
            if (std::is_integral<T>::value || std::is_same<T, Valuable>::value)
                out << 'v';
            return out << c;
        }
        
        const Variable& Host(const any::any& id) override {
            auto idT = any::any_cast<T>(id);
            auto it = hosted.find(idT);
            if(it == hosted.end())
                it = hosted.insert({idT, New(idT)}).first;
            return it->second;
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
} /* namespace math */
} /* namespace omnn */
