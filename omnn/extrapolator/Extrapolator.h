//
// Created by Сергей Кривонос on 10.08.17.
//
#pragma once
#include <algorithm>
#include <initializer_list>
#include <limits>
#include <limits.h>
#include <type_traits>
#include "Equation.h"
#include "Expression.h"
#include "FormulaOfVaWithSingleIntegerRoot.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>


namespace omnn{
namespace extrapolator{

    namespace ublas = boost::numeric::ublas;

    using extrapolator_base_matrix = boost::numeric::ublas::matrix<Valuable>;
    

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

    solution_t Solve(const ublas::vector<T>& augment) const
    {
        auto e = *this;
        auto sz1 = size1();
        auto sz2 = size2();
        
        ublas::vector<T> a(sz2);
        const ublas::vector<T>* au = &augment;
        if (sz1 > sz2 + 1/*augment*/)
        {
            //make square matrix to make it solvable by boost ublas
            e = Extrapolator(sz2, sz2);
            // sum first equations
            a[0] = 0;
            for (auto i = sz2; i--; )
            {
                e(0, i) = 0;
            }
            auto d = sz1-sz2;
            for (auto i = d; i--; )
            {
                for(auto j = sz2; j--; )
                {
                    e(0,j) += operator()(i,j);
                }
                a[0] += augment[i];
            }
            
            for(auto i = d; i<sz1; ++i)
            {
                for(auto j = sz2; j--; )
                {
                    e(i-d, j) = operator()(i,j);
                }
                a[i-d] = augment[i];
            }
            au = &a;
        }
        solution = ublas::solve(e, *au, ublas::upper_tag());
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
    bool Consistent(const ublas::vector<T>& augment)
    {
        auto sz = augment.size();
        extrapolator_base_matrix augmentedMatrix(sz, 1);
        for (auto i = sz; i-->0; ) {
            augmentedMatrix(i,0) = augment[i];
        }
        return Consistent(augmentedMatrix);
    }
    
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

    /**
     * Build matrix as column,raw,value
     */
    Extrapolator ViewMatrix() const
    {
        auto szy = size1();
        auto szx = size2();
        auto len = szx*szy;
        size_t i=0;
        Extrapolator e(len, 3); // column, row, value
        for (auto y = 0; y < szy; ++y) {
            for (auto x = 0; x < szx; ++x) {
                e(i, 0) = y;
                e(i, 1) = x;
                e(i, 2) = operator()(y,x);
                ++i;
            }
        }
        return e;
    }
    
    /**
     * @returns equivalent expression
     */
    operator Valuable() const
    {
        Valuable v = 0_v;
        auto szy = size1();
        auto szx = size2();
        std::vector<Variable> vars(szx);
        for(;szy--;)
        {
            Valuable s = 0_v;
            for(auto c = szx; c--;)
            {
                s += vars[c] * operator()(szy, c);
            }
            v += s*s;
        }
        v.optimize();
        return v;
    }
    
    Valuable Factors(const Variable& row, const Variable& col, const Variable& val) const;
    
    /**
     * Build formula of its ViewMatrix
     * @returns formula from two params
     */
    operator Formula() const;

    Valuable Equation(const ublas::vector<T>& augmented)
    {
        Valuable v = 0_v;
        auto szy = size1();
        auto szx = size2();
        std::vector<Variable> vars(szx);
        for(;szy--;)
        {
            Valuable s = 0_v;
            for(auto c = szx; c--;)
            {
                s += vars[c] * operator()(szy, c);
            }
            s -= augmented[szy];
            v += s*s;
        }
        v.optimize();
        return v;
    }
};

}
}
