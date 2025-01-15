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
    
    // Base value type
    using extrapolator_value_type = Valuable;

 
    // Allocator and container types
    using extrapolator_allocator_type = omnn::rt::custom_allocator<extrapolator_value_type>;
    using extrapolator_array_type = ublas::unbounded_array<extrapolator_value_type, extrapolator_allocator_type>;
    
    // Matrix and vector types
    using extrapolator_base_matrix = ublas::matrix<extrapolator_value_type, ublas::row_major, extrapolator_array_type>;
    using extrapolator_vector_type = ublas::vector<extrapolator_value_type, extrapolator_array_type>;
    using vector_type = extrapolator_vector_type;

    


class Extrapolator
        : public extrapolator_base_matrix
{
public:
    // Type definitions
    using base = extrapolator_base_matrix;
    using matrix_type = base;
    using vector_type = extrapolator_vector_type;
    using value_type = typename base::value_type;
    using size_type = typename base::size_type;
    using reference = typename base::reference;
    using const_reference = typename base::const_reference;
    using this_type = Extrapolator;
    using iterator1 = typename base::iterator1;
    using iterator2 = typename base::iterator2;
    using const_iterator1 = typename base::const_iterator1;
    using const_iterator2 = typename base::const_iterator2;
    using solution_t = vector_type;
    using extrapolator_vector = vector_type;

private:
    mutable solution_t solution;
    
    // Add const-correct swap operation
    friend void swap(Extrapolator& lhs, Extrapolator& rhs) noexcept {
        using std::swap;
        swap(static_cast<base&>(lhs), static_cast<base&>(rhs));
        swap(lhs.solution, rhs.solution);
    }

public:
    // Inherit base matrix functionality
    using base::operator();
    using base::size1;
    using base::size2;
    using base::operator=;
    using base::operator+=;
    using base::operator-=;
    using base::operator*=;
    using base::operator/=;
    using base::find1;
    using base::find2;
    using base::data;

    // Iterator support
    iterator1 begin1() { return base::begin1(); }
    const_iterator1 begin1() const { return base::begin1(); }
    iterator1 end1() { return base::end1(); }
    const_iterator1 end1() const { return base::end1(); }
    
    iterator2 begin2() { return base::begin2(); }
    const_iterator2 begin2() const { return base::begin2(); }
    iterator2 end2() { return base::end2(); }
    const_iterator2 end2() const { return base::end2(); }
    
    // Inherit constructors
    using base::base;
    
    // Add constructor from initializer list
    Extrapolator(std::initializer_list<std::vector<value_type>> dependancy_matrix);
    
    // Constructors
    Extrapolator() = default;
    
public:
    // Constructor for size
    Extrapolator(size_t rows, size_t cols) : base(rows, cols) {}

    solution_t Solve(const vector_type& augment) const;

    auto Determinant() const -> value_type;

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

    // Vector conversion operators
    template<typename OtherAlloc>
    operator ublas::vector<extrapolator_value_type, ublas::unbounded_array<extrapolator_value_type, OtherAlloc>>() const {
        using other_vector = ublas::vector<extrapolator_value_type, ublas::unbounded_array<extrapolator_value_type, OtherAlloc>>;
        other_vector result(this->size1());
        for (size_type i = 0; i < this->size1(); ++i) {
            result(i) = (*this)(i);
        }
        return result;
    }
    
    template<typename OtherAlloc>
    Extrapolator& operator=(const ublas::vector<extrapolator_value_type, ublas::unbounded_array<extrapolator_value_type, OtherAlloc>>& other) {
        this->resize(other.size(), 1);
        for (size_type i = 0; i < other.size(); ++i) {
            (*this)(i, 0) = other(i);
        }
        return *this;
    }

    Valuable Equation(const vector_type& augmented);
};

}
