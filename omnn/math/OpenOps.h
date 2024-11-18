//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once

#include <boost/operators.hpp>
#include <type_traits>
#include "Valuable.h"

namespace omnn {
namespace math {

    template<class CT>
    class OpenOps
            : public boost::operators<CT>
    {
    protected:
        // Helper to convert primitive types to CT
        template<typename T>
        static CT make_valuable(const T& x) {
            if constexpr (std::is_arithmetic_v<T>) {
                return CT(x);  // Assuming CT has constructor from arithmetic types
            } else {
                return static_cast<const CT&>(x);
            }
        }

    public:
        template <class Fwd>
        CT& operator-=(Fwd&& v) { return *static_cast<CT*>(this) += -std::forward<Fwd>(v); }

        template<class T>
        friend bool operator<=(const T &x, const CT &y) {
            return make_valuable(x) <= y;
        }

        template<class T>
        friend bool operator>=(const T &x, const CT &y) {
            return make_valuable(x) >= y;
        }

        template<class T>
        friend bool operator<(const T &x, const CT &y) {
            return make_valuable(x) < y;
        }

        template<class T>
        friend bool operator<=(const CT &x, const T &y) {
            return x <= make_valuable(y);
        }

        template<class T>
        friend bool operator>=(const CT &x, const T &y) {
            return x >= make_valuable(y);
        }

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
