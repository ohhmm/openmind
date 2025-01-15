//
// Created by Сергей Кривонос on 10.08.17.
//
#pragma once

#include <omnn/math/FormulaOfVaWithSingleIntegerRoot.h>

#include <omnn/rt/custom_allocator.hpp>


#include <algorithm>
#include <initializer_list>
#include <limits>
#include <limits.h>
#include <type_traits>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>


namespace omnn::math {

    namespace ublas = boost::numeric::ublas;
    //using extrapolator_value_type = a_rational;
    using extrapolator_value_type = Valuable;
    using extrapolator_allocator = custom_allocator<extrapolator_value_type>;
    using extrapolator_array = ublas::unbounded_array<extrapolator_value_type, extrapolator_allocator>;
    using extrapolator_base_matrix = ublas::matrix<extrapolator_value_type, ublas::row_major, extrapolator_array>;
    using extrapolator_vector_array = ublas::unbounded_array<extrapolator_value_type, extrapolator_allocator>;
    using extrapolator_vector = ublas::vector<extrapolator_value_type, extrapolator_vector_array>;


class Extrapolator
        : public extrapolator_base_matrix
{
    using base = extrapolator_base_matrix;
public:
    using T = extrapolator_value_type;
    using this_type = Extrapolator;
    using value_type = T;
    using size_type = typename base::size_type;
    using const_reference = typename base::const_reference;
    using reference = typename base::reference;
    using matrix_type = base;

public:
    using base::operator();
    using base::size1;
    using base::size2;
    using base::operator=;

public:
    using vector_type = extrapolator_vector;
    using solution_t = typename boost::numeric::ublas::matrix_vector_solve_traits<matrix_type, vector_type>::result_type;
    
    using base::base;  // Inherit constructors
    
    // Add constructor from initializer list
    Extrapolator(std::initializer_list<std::vector<T>> dependancy_matrix);
    
    // Add constructor for size
    Extrapolator(size_t rows, size_t cols) : base(rows, cols) {}

    mutable solution_t solution;

public:
	Extrapolator() = default;

    solution_t Solve(const vector_type& augment) const;

    auto Determinant() const -> T;

    /**
     * If possible, make the matrix consistent
     * @return true if it is
     * **/
    bool Consistent(const vector_type& augment);

    /**
     * If possible, make the matrix consistent
     * @return true if it is
     * **/
    bool Consistent(const this_type& augment);

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

    Valuable Equation(const vector_type& augmented);
};

}
