//
// Created by Сергей Кривонос on 10.08.17.
//

#pragma once

#include <initializer_list>
#include <boost/numeric/ublas/matrix.hpp>

namespace omnn{
namespace extrapolator{

using extrapolator_base_matrix = boost::numeric::ublas::matrix<int>;

template<class T = int>
class Extrapolator
        : public boost::numeric::ublas::matrix<T>
{
    using base = boost::numeric::ublas::matrix<T>;
public:
    using base::base;

    Extrapolator(std::initializer_list<std::vector<T>> list)
            : base(list.size(), list.begin()->size())
    {
        auto rows = list.size();
        auto r = list.begin();
        auto columns = r->size();
        for (unsigned i = 0; i < rows; ++ i)
        {
            auto c = r->begin();
            for (unsigned j = 0; j < columns; ++j)
            {
                (*this)(i, j) = *c;
                ++c;
            }
            ++r;
        }
    }

    T Determinant();

    /**
     * If possible, make the matrix consistent
     * @return is consistent
     * **/
    bool Consistent()
    {
        // TODO : check
        return true;
    }
};

}
}
