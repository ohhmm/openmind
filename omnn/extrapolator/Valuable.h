//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include "OpenOps.h"

namespace omnn{
namespace extrapolator {

class Valuable
        : public OpenOps<Valuable>
{
	using self = Valuable;
public:
    virtual ~Valuable();
    virtual self operator -(const self& v);
    virtual Valuable& operator +=(const Valuable& number);
    virtual Valuable& operator *=(const Valuable& number);
    virtual Valuable& operator /=(const Valuable& number);
    virtual Valuable& operator %=(const Valuable& number);
	virtual Valuable operator--(int);
};

}}
