//
// Created by Сергей Кривонос on 10.08.17.
//

#pragma once

#include <initializer_list>
#include <limits>
#include <limits.h>
#include <type_traits>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>

namespace omnn{
namespace extrapolator{

namespace ublas = boost::numeric::ublas;

template<typename ValType>
ValType det_fast(const ublas::matrix<ValType>& matrix)
{
    // create a working copy of the input
    using calc_matr_t = std::conditional<
            std::is_floating_point<ValType>::value,
            ValType,
            double>;
    static_assert( std::numeric_limits<calc_matr_t>::min() <= std::numeric_limits<ValType>::min() &&
                   std::numeric_limits<calc_matr_t>::max() >= std::numeric_limits<ValType>::max(),
                   "use long double for this type or add arbitrary aritmetics implementation" );
    ublas::matrix<calc_matr_t> mLu(matrix);
    ublas::permutation_matrix<std::size_t> pivots(matrix.size1());

    auto isSingular = ublas::lu_factorize(mLu, pivots);
    if (isSingular)
        return static_cast<ValType>(0);

    ValType det = static_cast<ValType>(1);
    for (std::size_t i = 0; i < pivots.size(); ++i)
    {
        if (pivots(i) != i)
            det *= static_cast<ValType>(-1);

        det *= mLu(i, i);
    }

    return det;
}

using extrapolator_base_matrix = boost::numeric::ublas::matrix<int>;

template<class T = int>
class Extrapolator
        : public boost::numeric::ublas::matrix<T>
{
    using base = boost::numeric::ublas::matrix<T>;

public:
    using base::base;

    Extrapolator(std::initializer_list<std::vector<T>>&& list)
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

    T Determinant()
    {
        using calc_matr_t = typename std::conditional<
                std::is_floating_point<T>::value,
                T,
                long double>::type;
        static_assert( -std::numeric_limits<calc_matr_t>::max() <= std::numeric_limits<T>::min() &&
                       std::numeric_limits<calc_matr_t>::max() >= std::numeric_limits<T>::max(),
                       "use long double for this type or add arbitrary aritmetics implementation" );
        ublas::matrix<calc_matr_t> mLu(*this);
        ublas::permutation_matrix<std::size_t> pivots(this->size1());

        auto isSingular = ublas::lu_factorize(mLu, pivots);
        if (isSingular)
            return static_cast<T>(0);

        T det = static_cast<T>(1);
        for (std::size_t i = 0; i < pivots.size(); ++i)
        {
            if (pivots(i) != i)
                det *= static_cast<T>(-1);

            det *= mLu(i, i);
        }

        return det;

    }

    /**
     * If possible, make the matrix consistent
     * @return is consistent
     * **/
    bool Consistent()
    {
        using calc_matr_t = typename std::conditional<
                std::is_floating_point<T>::value,
                T,
                long double>::type;
        static_assert( -std::numeric_limits<calc_matr_t>::max() <= std::numeric_limits<T>::min() &&
                       std::numeric_limits<calc_matr_t>::max() >= std::numeric_limits<T>::max(),
                       "use long double for this type or add arbitrary aritmetics implementation" );
        ublas::matrix<calc_matr_t> mLu(*this);
        auto r = ublas::solve(*this, ublas::vector<T>(this->size1()), ublas::upper_tag());
        return true;
    }
};

}
}
