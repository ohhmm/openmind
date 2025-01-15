//
// Created by Сергей Кривонос on 10.08.17.
//

#include "Extrapolator.h"

#include "omnn/math/Integer.h"
#include "omnn/math/Sum.h"

namespace omnn::math {

namespace ublas = boost::numeric::ublas;

namespace {
    auto det_fast(const Extrapolator& matrix)
    {
        using T = Extrapolator::value_type;
        using allocator_type = omnn::rt::custom_allocator<std::size_t>;
        using array_type = ublas::unbounded_array<std::size_t, allocator_type>;
        using perm_matrix_type = ublas::permutation_matrix<std::size_t, array_type>;
        perm_matrix_type pivots(matrix.size1());

        matrix_type tmp(matrix);
    auto isSingular = ublas::lu_factorize(tmp, pivots);
        if (isSingular)
            return T(0);

        T det = 1;
        for (std::size_t i = 0; i < pivots.size(); ++i)
        {
            if (pivots(i) != i)
                det *= static_cast<double>(-1);

            det *= matrix(i, i);
        }

        return det;
    }
} // anonymous namespace

bool Extrapolator::Consistent(const this_type& augment)
{
    return Determinant() == det_fast(augment);
}

Valuable Extrapolator::Factors(const Variable& row, const Variable& col, const Variable& val) const
{
    using size_type = typename base::size_type;
    Product e;
    size_type szy = this->size1();
    size_type szx = this->size2();
    Valuable::OptimizeOff off;
    for (size_type y = 0; y < szy; ++y) {
        for (size_type x = 0; x < szx; ++x) {
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
    using size_type = typename base::size_type;
    for (size_type i = 0; i < vm.size1(); ++i) {
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
    using size_type = typename base::size_type;
    auto it = dependancy_matrix.begin();
    for (size_type i = 0; i < dependancy_matrix.size(); ++i, ++it) {
        const auto& row = *it;
        for (size_type j = 0; j < row.size(); ++j) {
            (*this)(i, j) = row[j];
        }
    }
}
Extrapolator::solution_t Extrapolator::Solve(const extrapolator_vector& augment) const
{
    using size_type = typename extrapolator_base_matrix::size_type;
    using vector_type = extrapolator_vector;
    auto e = *this;
    size_type sz1 = this->size1();
    size_type sz2 = this->size2();

    vector_type a(sz2);
    const vector_type* au = &augment;
    if (sz1 > sz2 + 1 /*augment*/) {
        // make square matrix to make it solvable by boost ublas
        e = Extrapolator(sz2, sz2);
        // sum first equations
        a[0] = 0;
        for (size_type i = 0; i < sz2; ++i) {
            e(0, i) = 0;
        }
        auto d = sz1 - sz2;
        for (size_type i = 0; i < d; ++i) {
            for (size_type j = 0; j < sz2; ++j) {
                e(0, j) += (*this)(i, j);
            }
            a[0] += augment[i];
        }

        for (size_type i = d; i < sz1; ++i) {
            for (size_type j = 0; j < sz2; ++j) {
                e(i - d, j) = (*this)(i, j);
            }
            a[i - d] = augment[i];
        }
        au = &a;
    }
    solution = ublas::solve(e, *au, ublas::upper_tag());
    return solution;
}

Extrapolator::T Extrapolator::Determinant() const
{
    using allocator_type = omnn::rt::custom_allocator<std::size_t>;
    using array_type = ublas::unbounded_array<std::size_t, allocator_type>;
    using perm_matrix_type = ublas::permutation_matrix<std::size_t, array_type>;
    perm_matrix_type pivots(this->size1());
    auto mLu = *this;
    matrix_type tmp(mLu);
    auto isSingular = ublas::lu_factorize(tmp, pivots);
    if (isSingular)
        return static_cast<T>(0);

    T det = static_cast<T>(1);
    for (std::size_t i = 0; i < pivots.size(); ++i) {
        if (pivots(i) != i)
            det *= static_cast<T>(-1);

        det *= mLu(i, i);
    }

    return det;
}

bool Extrapolator::Consistent(const extrapolator_vector& augment)
{
    using size_type = typename extrapolator_base_matrix::size_type;
    auto sz = augment.size();
    Extrapolator augmentedMatrix(sz, 1);
    for (size_type i = 0; i < sz; ++i) {
        augmentedMatrix(i, 0) = augment[i];
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
    const auto szy = this->size1();
    const auto szx = this->size2();
    const auto len = szx * szy;
    size_type i = 0;
    Extrapolator e(len, 3); // column, row, value
    for (size_type y = 0; y < szy; ++y) {
        for (size_type x = 0; x < szx; ++x) {
            e(i, 0) = value_type(y);
            e(i, 1) = value_type(x);
            e(i, 2) = (*this)(y, x);
            ++i;
        }
    }
    return e;
}

Extrapolator::operator Valuable() const
{
    Valuable v = 0_v;
    const auto szy = this->size1();
    const auto szx = this->size2();
    std::vector<Variable> vars(szx);
    for (size_type y = 0; y < szy; ++y) {
        Valuable s = 0_v;
        for (size_type c = 0; c < szx; ++c) {
            s += vars[c] * (*this)(y, c);
        }
        v += s * s;
    }
    v.optimize();
    return v;
}

Valuable Extrapolator::Equation(const vector_type& augmented)
{
    Valuable v = 0_v;
    const auto szy = this->size1();
    const auto szx = this->size2();
    std::vector<Variable> vars(szx);
    for (size_type y = 0; y < szy; ++y) {
        Valuable s = 0_v;
        for (size_type c = 0; c < szx; ++c) {
            s += vars[c] * (*this)(y, c);
        }
        s -= augmented[y];
        v += s * s;
    }
    v.optimize();
    return v;
} // namespace omnn::math
