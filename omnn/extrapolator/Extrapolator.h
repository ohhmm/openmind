//
// Created by Сергей Кривонос on 10.08.17.
//
#pragma once
#include <algorithm>
#include <initializer_list>
#include <limits>
#include <limits.h>
#include <type_traits>

#include <omnn/math/FormulaOfVaWithSingleIntegerRoot.h>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>


namespace omnn::math {

    namespace ublas = boost::numeric::ublas;
    //using extrapolator_value_type = a_rational;
    using extrapolator_value_type = Valuable;
    using extrapolator_base_matrix = boost::numeric::ublas::matrix<extrapolator_value_type>;


class Extrapolator
        : public extrapolator_base_matrix
{
    using base = extrapolator_base_matrix;
    using T = extrapolator_base_matrix::value_type;

    using solution_t = typename ublas::matrix_vector_solve_traits<base, ublas::vector<T>>::result_type;

    mutable solution_t solution;

public:
    using base::base;

	Extrapolator() = default;

    Extrapolator(std::initializer_list<std::vector<T>> dependancy_matrix);

    solution_t Solve(const ublas::vector<T>& augment) const;

    T Determinant() const;

    /**
     * If possible, make the matrix consistent
     * @return true if it is
     * **/
    bool Consistent(const ublas::vector<T>& augment);

    /**
     * If possible, make the matrix consistent
     * @return true if it is
     * **/
    bool Consistent(const extrapolator_base_matrix& augment);

    /**
     * this is to complete it with other deducible variables
     * @param e expression with known varable values INCLUDING ZEROS
     * @return expression with additional deducted variables
     */
    Valuable Complete(const Valuable& e);

    /**
     * this is to complete it with other deducible variables
     * @param e expression with known varable values INCLUDING ZEROS
     * @return expression with additional deducted variables
     */
//    template<template <class> class RawContainerT>
//    Expression Complete(const RawContainerT<T>& data) {
//        // search for the row
//        auto it1 = this->begin1();
//        for (; it1 != this->end1(); ++it1) {
//            if (std::equal(data.begin(), data.end(), it1.begin())) {
//                break; // found it1
//            }
//        }
//    }

    /**
     * Build matrix as column,raw,value
     */
    Extrapolator ViewMatrix() const;

    /**
     * @returns equivalent expression
     */
    operator Valuable() const;

    Valuable Factors(const Variable& row, const Variable& col, const Variable& val) const;

    /**
     * Build formula of its ViewMatrix
     * @returns formula from two params
     */
    operator Formula() const;

    Valuable Equation(const ublas::vector<T>& augmented);
};

}
