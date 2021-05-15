//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once

#include <boost/operators.hpp>

namespace omnn {
namespace math {

    template<class CT>
    class OpenOps
            : public boost::operators<CT>
    {
    public:
        template <class Fwd>
        CT& operator-=(Fwd&& v) { return *static_cast<CT*>(this) += -std::forward<Fwd>(v); }

        template<class T>
        friend bool operator<=(const T &x, const CT &y) { return !static_cast<bool>(x > y); }

        template<class T>
        friend bool operator>=(const T &x, const CT &y) { return !static_cast<bool>(x < y); }

//        template<class T>
//        friend bool operator>(const CT &x, const T &y) { return y < x; }

        template<class T>
        friend bool operator<(const T &x, const CT &y) { return y > x; }

        template<class T>
        friend bool operator<=(const CT &x, const T &y) { return !static_cast<bool>(y < x); }

        template<class T>
        friend bool operator>=(const CT &x, const T &y) { return !static_cast<bool>(y > x); }

//        template<class T>
//        friend CT operator-(const CT &x, const T &y) { return x - y; }

//        template<class T>
//        friend T operator-(const T &x, const CT &y) { return x - y; }

//        template<class T>
//        friend CT operator+(const CT &x, const T &y) { return CT(x) += y; }
//        
//        template<class T>
//        friend T operator+(const T &x, const CT &y) { return x + y; }

//        template<class T>
//        friend T operator+(const T &x, const T &y) { return x + y; }
    };

}}
