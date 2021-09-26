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


using namespace omnn::math;

using namespace std::string_literals;



const any::any& omnn::math::VarHost::GetId(const Variable& va) const {
	return va.GetId();
}


template<>
any::any TypedVarHost<std::string>::NewVarId()
{
    static boost::uuids::random_generator UuidGen;
    auto id = UuidGen();
    auto s = boost::lexical_cast<std::string>(id);
    boost::erase_all(s, "-");
    s.insert(0, "va_");
    return std::move(s);
}

template <>
std::string_view TypedVarHost<std::string>::GetName(const any::any& v) const
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
std::string_view TypedVarHost<Valuable>::GetName(const any::any& v) const
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
