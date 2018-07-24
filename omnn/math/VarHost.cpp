/*
 * VarHost.cpp
 *
 *  Created on: 25 авг. 2017 г.
 *      Author: sergejkrivonos
 */

#include "VarHost.h"
#include <boost/uuid/uuid.hpp>

using namespace omnn::math;

template<>
any::any TypedVarHost<std::string>::NewVarId()
{
    return boost::uuids::uuid();
}

template<>
void VarHost::inc<>(std::string&)
{
    IMPLEMENT
}
