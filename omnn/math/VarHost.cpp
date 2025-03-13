/*
 * VarHost.cpp
 *
 *  Created on: 25 авг. 2017 г.
 *      Author: sergejkrivonos
 */

#include "VarHost.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <string_view>


using namespace std::string_literals;


namespace omnn::math {

thread_local bool VarHost::add_non_zero_mode_on = true;

const ::std::any& VarHost::GetId(const Variable& va) const { return va.GetId(); }

Variable VarHost::New(const ::std::any& id) {
    Variable v(shared_from_this());
    v.SetId(id);
    return v;
}

Variable VarHost::New() { return New(NewVarId()); }

template<>
::std::any TypedVarHost<std::string>::NewVarId()
{
    static boost::uuids::random_generator UuidGen;
    auto id = UuidGen();
    auto s = boost::lexical_cast<std::string>(id);
    boost::erase_all(s, "-");
    s.insert(0, "va_");
    return std::move(s);
}

template <>
std::string_view TypedVarHost<std::string>::GetName(const ::std::any& v) const
{
    auto& storage = const_cast<TypedVarHost<std::string>*>(this)->HostedStorage(v);
    auto& strCache = const_cast<std::string&>(storage.second);
    if (strCache.empty()) {
        auto& va = storage.first;
        strCache = GetId(va);
    }
    return strCache;
}

template <>
std::string_view TypedVarHost<Valuable>::GetName(const ::std::any& v) const
{
    auto& storage = const_cast<TypedVarHost<Valuable>*>(this)->HostedStorage(v);
    auto& strCache = const_cast<std::string&>(storage.second);
    if (strCache.empty()) {
        auto& va = storage.first;
        strCache = GetId(va).str();
        strCache.insert(0, "v");
	}
    return strCache;
}

template<>
void VarHost::inc<>(std::string&)
{
    IMPLEMENT
}

void VarHost::LogNotZero(const Valuable& value) {
    if (add_non_zero_mode_on) {
        LogNotZeroBypassScopes(value);
    }
}

void VarHost::LogNotZeroBypassScopes(const Valuable& v) {
    if (v.IsZero()) {
        throw std::runtime_error("Variable is zero");
    }
    else if (v.IsSimple() && v.is_optimized()) {
	}
    else {
        if (!v.FindVa()) {
            if (!v.is_optimized()) {
                Valuable::OptimizeOn enable;
                auto copy = v;
                copy.optimize();
                if (copy.IsZero()) {
					throw std::runtime_error("Variable is zero");
                }
                else if (copy.IsSimple()) {
                    return;
                }
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
                else {
                    LOG_AND_IMPLEMENT("LogNotZero " << v << "  =  " << copy);
                }
#endif
            }
        }
        nonZeroItems[v.Vars()].emplace(v);
    }
}

namespace {
    // Template function to extract keys from a map and store them in a set
    template <typename K, typename V>
    std::set<K> Keys(const std::map<K, V>& inputMap) {
        std::set<K> resultSet;
        for (const auto& element : inputMap) {
            resultSet.insert(element.first);
        }
        return resultSet;
    }
}
bool VarHost::TestRootConsistencyWithNonZeroLog(const Valuable::vars_cont_t& values) const {
    auto ok = true;
    auto keys = Keys(values);
    auto it = nonZeroItems.find(keys);
    if (it != nonZeroItems.end()) {
        Valuable::OptimizeOn optimizeOn;
        for (auto inequalityToZero : it->second) {
            if (inequalityToZero.eval(values)) {
                inequalityToZero.optimize();
                ok = !inequalityToZero.IsZero();
                if (!ok)
                    break;
            }
		}
    }
    return ok;
}

bool VarHost::TestRootConsistencyWithNonZeroLog(const Variable& variable, const Valuable& value) const {
    return nonZeroItems.empty()
        || TestRootConsistencyWithNonZeroLog({{variable, value}});
}

} // namespace
