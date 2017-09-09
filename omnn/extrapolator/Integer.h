//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/numeric/conversion/converter.hpp>
#include "Valuable.h"

namespace omnn{
namespace extrapolator {

class Integer
    : public ValuableDescendantContract<Integer>
{
    using base = ValuableDescendantContract<Integer>;
    using base_int = boost::multiprecision::cpp_int;
    using const_base_int_ref = const base_int&;
    base_int arbitrary = 0;
public:
    using base::base;

    template<class T>
    Integer(const T& i) {
        arbitrary = i;
    }

    Integer(base_int&& i) : arbitrary(i)
    {
    }
    
    Integer(int&& i) : arbitrary(i)
    {
    }
    
    operator base_int() const {
        return arbitrary;
    }
    
    operator int() const {
        return boost::numeric_cast<int>(arbitrary);
    }

    Valuable operator -(const Valuable& v) const override;
    Valuable& operator +=(const Valuable& v) override;
    Valuable& operator *=(const Valuable& v) override;
    Valuable& operator /=(const Valuable& v) override;
    Valuable& operator %=(const Valuable& v) override;
    Valuable& operator --() override;
    Valuable& operator ++() override;
    
    bool operator <(const Valuable& v) const override;
            
//    bool operator>(int v) const  {
//        return arbitrary > v;
//    }
            
    bool operator ==(const Valuable& v) const override;
            
    friend std::ostream& operator <<(std::ostream& out, const Integer& obj);
};


}
}

//template<class T>
//bool operator>(const omnn::extrapolator::Integer& integer, int i)
//{
//    
//}
