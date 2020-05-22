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

using namespace omnn::math;


template<>
any::any TypedVarHost<std::string>::NewVarId()
{
    static boost::uuids::random_generator UuidGen;
    auto id = UuidGen();
    auto s = boost::lexical_cast<std::string>(id);
    boost::erase_all(s, "-");
    static std::string prefix = "va_";
    s.insert(0, prefix);
    return std::move(s);
}

template<>
void VarHost::inc<>(std::string&)
{
    IMPLEMENT
}
