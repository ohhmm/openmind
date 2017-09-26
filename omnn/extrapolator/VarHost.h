/*
 * VarHost.h
 *
 *  Created on: 25 авг. 2017 г.
 *      Author: sergejkrivonos
 */

#pragma once
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
    {
    public:
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
        virtual bool Has(any::any id) const;
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
    public:
        using VarHost::VarHost;
    
        virtual void AddNewVarById(void* id) {
            auto varId = dynamic_cast<T*>(id);
            if (varId)
            {
                varIds.insert(*varId);
            }
            else
            {
                throw "Wrong param";
            }
        }
        
        any::any NewVarId() override {
            const auto& last = *varIds.rbegin();
            if (std::is_same<std::string, T>::value) {
                // TODO : find first digit to extract var name and num
                throw "Implement";
                //int index =
                std::string name, num;
                boost::multiprecision::cpp_int n;
                std::stringstream(num) >> n;
                return (std::stringstream(name) << ++n).str();
            } else if (std::is_arithmetic<T>::value) {
                auto n = last;
                std::pair<T, bool> inserted;
                do{
                    inserted = varIds.insert(++n);
                }while(!inserted.second);
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
        
        std::ostream& print(std::ostream& out, any::any v) const {
            return out <<any::any_cast<T>(v);
        }
    };

} /* namespace extrapolator */
} /* namespace omnn */
