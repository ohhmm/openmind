/*
 * VarHost.h
 *
 *  Created on: 25 авг. 2017 г.
 *      Author: sergejkrivonos
 */

#pragma once
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <boost/any.hpp>
#include <boost/multiprecision/cpp_int.hpp>

namespace omnn {
namespace extrapolator {

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
        void AddVarId(const T& id) {
            auto host = dynamic_cast<TypedVarHost<T>*>(this);
            if (host) {
                host->AddNewVarById(&id);
            }
            else
            {
                throw "wrong id type";
            }
        }
    protected:
        VarHost() = default;
        virtual void AddNewVarById(void* id) {
            throw "Implement!";
        }
    public:
        ptr sh() {
            return shared_from_this();
        }
        template<class T>
        static ptr make(){
            return new TypedVarHost<T>();
        }
        template<class T = boost::multiprecision::cpp_int>
        static cref Global(){
            static TypedVarHost<T> host;
            return host;
        }
        virtual bool Has(any::any id) const = 0;
        virtual any::any NewVarId() = 0;
        virtual bool CompareIdsLess(any::any a, any::any b) const = 0;
        virtual bool CompareIdsEqual(any::any a, any::any b) const = 0;
        virtual std::ostream& print(std::ostream& out, any::any v) const = 0;
    };
    
    /**
     * ensures variable id uniquness in a space of variables
     */
    template<class T>
    class TypedVarHost : public VarHost
    {
        std::set<T> varIds;
        using VarHost::VarHost;
    public:

        void AddNewVarById(void* id) override {
            if (std::is_class<T>::value) {
                auto varId = static_cast<T*>(id);
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
        
        any::any NewVarId() override {

            T t = 0;
            const auto& last = varIds.size() ? *varIds.rbegin() : t;
            if (std::is_same<std::string, T>::value) {
                // TODO : find first digit to extract var name and num
                throw "Implement";
                //int index =
                std::string name, num;
                boost::multiprecision::cpp_int n;
                std::stringstream(num) >> n;
                std::stringstream s;
                s<< name << ++n;
                return s.str();
            } else if (std::is_arithmetic<T>::value || std::is_same<boost::multiprecision::cpp_int, T>::value) {
                auto n = last;
                typename decltype(varIds)::iterator inserted;
                while(varIds.find(++n)!=varIds.end());
                varIds.insert(n);
                return n;
            } else {
                throw "Implement!";
            }
            
        }
        
        bool Has(any::any id) const override {
            return varIds.find(any::any_cast<T>(id)) != varIds.end();
        }
        
        bool CompareIdsLess(any::any a, any::any b) const override {
            return any::any_cast<T>(a) < any::any_cast<T>(b);
        }
        
        bool CompareIdsEqual(any::any a, any::any b) const override {
            return any::any_cast<T>(a) == any::any_cast<T>(b);
        }
        
        std::ostream& print(std::ostream& out, any::any v) const override {
            return out <<any::any_cast<T>(v);
        }
    };

} /* namespace extrapolator */
} /* namespace omnn */
