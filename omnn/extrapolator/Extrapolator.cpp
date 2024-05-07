//
// Created by Сергей Кривонос on 10.08.17.
//

#include "Extrapolator.h"

#include "omnn/math/Integer.h"
#include "omnn/math/Sum.h"

#include <boost/numeric/ublas/storage.hpp>


using namespace omnn;
using namespace math;
using namespace boost::numeric::ublas;


// Ensure that matrix_t, vector_t, and permutation_matrix_t are using unbounded_array_wrapper with custom_allocator
using matrix_t = boost::numeric::ublas::matrix<Valuable, boost::numeric::ublas::basic_row_major<>, unbounded_array_wrapper<Valuable, custom_allocator<Valuable>>>;
using vector_t = boost::numeric::ublas::vector<Valuable, unbounded_array_wrapper<Valuable, custom_allocator<Valuable>>>;
using permutation_matrix_t = boost::numeric::ublas::permutation_matrix<std::size_t, unbounded_array_wrapper<std::size_t, custom_allocator<std::size_t>>>;

// This function calculates the determinant of a matrix using LU factorization
auto det_fast(matrix_t matrix) {
    // Use the matrix's size1 directly to avoid calling size on the allocator
    auto matrix_size = matrix.size1();
    permutation_matrix_t pivots(matrix_size);

    // Perform LU factorization on a copy of the matrix to preserve the original matrix
    matrix_t matrix_copy(matrix);
    auto isSingular = ublas::lu_factorize(matrix_copy, pivots);
    if (isSingular)
        return Valuable(0);

    Valuable det(1);
    for (std::size_t i = 0; i < matrix_size; ++i) {
        if (pivots(i) != i)
            det *= Valuable(-1);

        det *= matrix_copy(i, i);
    }

    return det;
}

bool Extrapolator::Consistent(const matrix_t& augment) {
    return Determinant() == det_fast(augment);
}

Valuable Extrapolator::Factors(const Variable& row, const Variable& col, const Variable& val) const
{
    Product e;
    auto szy = size1();
    auto szx = size2();
    Valuable::OptimizeOff off;
    for (auto y = 0; y < szy; ++y) {
        for (auto x = 0; x < szx; ++x) {
            e.Add(((row-y)^2)
                  +((col-x)^2)
                  +((val-(*this)(y,x))^2));
        }
    }
    return e;
}

Extrapolator::operator Formula() const
{
    bool integers = true;
    auto vm = ViewMatrix();
    Valuable e = 1_v;
    Variable vx,vy,vv;
    Valuable::optimizations = false;
    for (auto i = vm.size1(); i--; ) {
        Valuable v = vm(i,2);
        integers = integers && v.IsInt();
        auto e1 = vx - vm(i,0);
        auto e2 = vy - vm(i,1);
        auto e3 = vv - v;
        auto subsyst = e1*e1 + e2*e2 + e3*e3;
        e *= subsyst;
    }
    Valuable::optimizations = true;
    e.optimize();
    if(!e.IsSum())
        throw "Debug!";
    auto& s = e.as<Sum>();
    if(integers)
        return FormulaOfVaWithSingleIntegerRoot(vv, s);
    else
        return s.FormulaOfVa(vv);
}

Extrapolator::Extrapolator(std::initializer_list<std::vector<T>> dependancy_matrix)
    : base(dependancy_matrix.size(), dependancy_matrix.begin()->size())
{
    auto rows = dependancy_matrix.size();
    auto r = dependancy_matrix.begin();
    auto columns = r->size();
    for (unsigned i = 0; i < rows; ++i) {
        auto c = r->begin();
        for (unsigned j = 0; j < columns; ++j) {
            (*this)(i, j) = *c;
            ++c;
        }
        ++r;
    }
}
Extrapolator::solution_t Extrapolator::Solve(const vector_t& augment) const
{
    auto e = static_cast<const matrix_t&>(*this);
    auto sz1 = size1();
    auto sz2 = size2();

    // Initialize vector with size and default value
    vector_t a(sz2, Valuable(0));
    const vector_t* au = &augment;
    if (sz1 > sz2 + 1 /*augment*/) {
        // make square matrix to make it solvable by boost ublas
        e = Extrapolator(sz2, sz2);
        // sum first equations
        for (auto i = sz2; i--;) {
            e.insert_element(0, i, Valuable(0));
        }
        auto d = sz1 - sz2;
        for (auto i = d; i--;) {
            for (auto j = sz2; j--;) {
                e(0, j) += operator()(i, j);
            }
            a(i - d) = (*au)(i); // Use vector's operator() provided by Boost Ublas for element access
        }

        for (auto i = d; i < sz1; ++i) {
            for (auto j = sz2; j--;) {
                e(i - d, j) = operator()(i, j);
            }
            a(i - d) = (*au)(i); // Use vector's operator() provided by Boost Ublas for element access
        }
        au = &a;
    }
    solution = ublas::solve(e, *au, ublas::upper_tag());
    return solution;
}

Extrapolator::T Extrapolator::Determinant() const {
    permutation_matrix_t pivots(this->size1());
    auto mLu = *this;
    auto isSingular = ublas::lu_factorize(mLu, pivots);
    if (isSingular)
        return static_cast<T>(0);

    auto det = static_cast<T>(1);
    for (std::size_t i = 0; i < pivots.size(); ++i) {
        if (pivots(i) != i)
            det *= static_cast<T>(-1);

        det *= mLu(i, i);
    }

    return det;
}

bool Extrapolator::Consistent(const vector_t& augment) {
    auto sz = augment.size();
    matrix_t augmentedMatrix(sz, 1);
    for (auto i = sz; i-- > 0;) {
        augmentedMatrix(i, 0) = augment(i);
    }
    return Consistent(augmentedMatrix);
}

Valuable Extrapolator::Complete(const Valuable& e)
{
    // search for the row
    IMPLEMENT
}

Extrapolator Extrapolator::ViewMatrix() const
{
    auto szy = size1();
    auto szx = size2();
    auto len = szx * szy;
    size_t i = 0;
    Extrapolator e(len, 3); // column, row, value
    for (auto y = 0; y < szy; ++y) {
        for (auto x = 0; x < szx; ++x) {
            e(i, 0) = y;
            e(i, 1) = x;
            e(i, 2) = operator()(y, x);
            ++i;
        }
    }
    return e;
}

Extrapolator::operator Valuable() const
{
    Valuable v = 0_v;
    auto szy = size1();
    auto szx = size2();
    std::vector<Variable> vars(szx);
    for (; szy--;) {
        Valuable s = 0_v;
        for (auto c = szx; c--;) {
            s += vars[c] * operator()(szy, c);
        }
        v += s * s;
    }
    v.optimize();

    return v;
}

Valuable Extrapolator::Equation(const ublas::vector<T>& augmented)
{
    Valuable v = 0_v;
    auto szy = size1();
    auto szx = size2();
    std::vector<Variable> vars(szx);
    for (; szy--;) {
        Valuable s = 0_v;
        for (auto c = szx; c--;) {
            s += vars[c] * operator()(szy, c);
        }
        s -= augmented[szy];
        v += s * s;
    }
    v.optimize();
    return v;
}
