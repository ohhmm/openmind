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
    virtual self operator -(const self& v) const;
    virtual Valuable& operator +=(const Valuable& number);
    virtual Valuable& operator *=(const Valuable& number);
    virtual Valuable& operator /=(const Valuable& number);
    virtual Valuable& operator %=(const Valuable& number);
    virtual Valuable& operator--();
    virtual Valuable& operator++();
    virtual bool operator<(const Valuable& number) const;
    virtual bool operator==(const Valuable& number) const;
    
    template<class T>
    T* As()
    { return dynamic_cast<T*>(this); }
    
    friend std::ostream& operator<<(std::ostream& out, const Valuable& obj);
};

}}
