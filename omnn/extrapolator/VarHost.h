/*
 * VarHost.h
 *
 *  Created on: 25 авг. 2017 г.
 *      Author: sergejkrivonos
 */

#pragma once
#include <set>
namespace omnn {
namespace extrapolator {

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

};

} /* namespace extrapolator */
} /* namespace omnn */
