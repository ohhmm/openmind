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

protected:
    std::ostream& print(std::ostream& out) const override;

public:
	using base_int = boost::multiprecision::cpp_int;
	using const_base_int_ref = const base_int&;
	
	using base::base;

    Integer(const Integer&)=default;
    Integer(Integer&&)=default;
	Integer& operator=(const Integer& f) = default;
    Integer(int i = 0)
        : arbitrary(i)
    {
    }

    Integer(const base_int& i)
        : arbitrary(i)
    {
    }
    
    operator base_int() const {
        return arbitrary;
    }
    
//    operator int() const {
//        return boost::numeric_cast<int>(arbitrary);
//    }

    // virtual operators 
    Valuable operator -() const override;
    Valuable& operator +=(const Valuable& v) override;
    Valuable& operator +=(int v) override;
    Valuable& operator *=(const Valuable& v) override;
    Valuable& operator /=(const Valuable& v) override;
    Valuable& operator %=(const Valuable& v) override;
    Valuable& operator --() override;
    Valuable& operator ++() override;
    bool operator <(const Valuable& v) const override;
    bool operator ==(const Valuable& v) const override;
    
    // concrete operators
    bool operator <(const Integer& v) const { return arbitrary < v.arbitrary; }
    bool operator >(const Integer& v) const { return arbitrary > v.arbitrary; }
//    bool operator>(int v) const  {
//        return arbitrary > v;
//    }
            
            
    friend std::ostream& operator <<(std::ostream& out, const Integer& obj);

private:
	base_int arbitrary = 0;
};


}
}

