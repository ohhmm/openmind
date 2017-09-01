//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once

#include <boost/operators.hpp>

namespace omnn {
namespace extrapolator {

    template<class CT>
    class OpenOps
            : public boost::operators<CT>
    {
        template<class T>
        friend bool operator<=(const T &x, const CT &y) { return !static_cast<bool>(x > y); }

        template<class T>
        friend bool operator>=(const T &x, const CT &y) { return !static_cast<bool>(x < y); }

        template<class T>
        friend bool operator>(const CT &x, const T &y) { return y < x; }

        template<class T>
        friend bool operator<(const CT &x, const T &y) { return y > x; }

        template<class T>
        friend bool operator<=(const CT &x, const T &y) { return !static_cast<bool>(y < x); }

        template<class T>
        friend bool operator>=(const CT &x, const T &y) { return !static_cast<bool>(y > x); }
    };

}}