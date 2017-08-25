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

template<class T>
class VarHost : public std::set<T>
{

};

} /* namespace extrapolator */
} /* namespace omnn */
