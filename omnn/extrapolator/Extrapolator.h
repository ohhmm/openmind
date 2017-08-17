//
// Created by Сергей Кривонос on 10.08.17.
//

#pragma once
#include <algorithm>
#include <initializer_list>
#include <limits>
#include <limits.h>
#include <type_traits>
#include "Expression.h"
#include "Number.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>

namespace omnn{
namespace extrapolator{

namespace ublas = boost::numeric::ublas;


Number det_fast(ublas::matrix<Number> matrix)
{
    ublas::permutation_matrix<std::size_t> pivots(matrix.size1());

    auto isSingular = ublas::lu_factorize(matrix, pivots);
    if (isSingular)
        return static_cast<Number>(0);

    Number det = static_cast<Number>(1);
    for (std::size_t i = 0; i < pivots.size(); ++i)
    {
        if (pivots(i) != i)
            det *= static_cast<Number>(-1);

        det *= matrix(i, i);
    }

    return det;
}

using extrapolator_base_matrix = boost::numeric::ublas::matrix<int>;


class Extrapolator
        : public ublas::matrix<Number>
{
    using T = Number;
    using base = ublas::matrix<Number>;
    
    using solution_t = typename ublas::matrix_vector_solve_traits<base, ublas::vector<T>>::result_type;

    solution_t solution;

public:
    using base::base;


    Extrapolator(std::initializer_list<std::vector<T>>&& dependancy_matrix)
            : base(dependancy_matrix.size(), dependancy_matrix.begin()->size())
    {
        auto rows = dependancy_matrix.size();
        auto r = dependancy_matrix.begin();
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

    solution_t Solve(const ublas::vector<T>& augment){
        solution = ublas::solve(*this, augment, ublas::upper_tag());
        return solution;
    }

    T Determinant()
    {
        ublas::permutation_matrix<std::size_t> pivots(this->size1());
        auto mLu = *this;
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
     * @return true if it is
     * **/
    bool Consistent()
    {
        auto det = Determinant();
        //todo: implement
        //auto copy = this->
        //Solve();
        return true;
    }

    /**
     * this is to complete it with other deducible variables
     * @param e expression with known varable values INCLUDING ZEROS
     * @return expression with additional deducted variables
     */
    Expression Complete(const Expression& e)
    {
        // search for the row
        throw "Implement now!";
    }

    /**
     * this is to complete it with other deducible variables
     * @param e expression with known varable values INCLUDING ZEROS
     * @return expression with additional deducted variables
     */
    template<template <class> class RawContainerT>
    Expression Complete(const RawContainerT<T>& data) {
        // search for the row
        auto it1 = this->begin1();
        for (; it1 != this->end1(); ++it1) {
            if (std::equal(data.begin(), data.end(), it1.begin())) {
                break; // found it1
            }
        }


    }

    
};

}
}
