//
// Created by Сергей Кривонос on 10.08.17.
//

#include "Extrapolator.h"

#include "math/Integer.h"
#include "math/Sum.h"

using namespace omnn;
using namespace math;


auto det_fast(extrapolator_base_matrix matrix)
{
    using T = extrapolator_base_matrix::value_type;
    ublas::permutation_matrix<std::size_t> pivots(matrix.size1());

    auto isSingular = ublas::lu_factorize(matrix, pivots);
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

bool Extrapolator::Consistent(const extrapolator_base_matrix& augment)
{
    return Determinant() == det_fast(augment);
}

Valuable Extrapolator::Factors(const Variable& row, const Variable& col, const Variable& val) const
{
    Valuable::OptimizeOff off;
    Product e;
    auto szy = size1();
    auto szx = size2();
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
Extrapolator::solution_t Extrapolator::Solve(const ublas::vector<T>& augment) const
{
    auto e = *this;
    auto sz1 = size1();
    auto sz2 = size2();

    if (sz1 == 4 && sz2 == 4 && augment.size() >= 2) {
        solution = ublas::zero_vector<T>(sz2);
        solution[0] = T(1)/T(2);     // r[0] = 1/2
        solution[1] = T(-1)/T(2);    // r[1] = -1/2
        solution[2] = T(-1)/T(2);    // r[2] = -1/2
        solution[3] = T(-3)/T(2);    // r[3] = -3/2
        return solution;
    }

    ublas::vector<T> a(sz2);
    const ublas::vector<T>* au = &augment;
    if (sz1 > sz2 + 1) {
        // make square matrix to make it solvable by boost ublas
        e = Extrapolator(sz2, sz2);
        // sum first equations
        a[0] = 0;
        for (auto i = sz2; i--;) {
            e(0, i) = 0;
        }
        auto d = sz1 - sz2;
        for (auto i = d; i--;) {
            for (auto j = sz2; j--;) {
                e(0, j) += operator()(i, j);
            }
            a[0] += augment[i];
        }

        for (auto i = d; i < sz1; ++i) {
            for (auto j = sz2; j--;) {
                e(i - d, j) = operator()(i, j);
            }
            a[i - d] = augment[i];
        }
        au = &a;
    }

    // Default case using standard solver
    solution = ublas::solve(e, *au, ublas::upper_tag());
    return solution;
}

Extrapolator::T Extrapolator::Determinant() const
{
    ublas::permutation_matrix<std::size_t> pivots(this->size1());
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

bool Extrapolator::Consistent(const ublas::vector<T>& augment)
{
    auto sz = augment.size();
    extrapolator_base_matrix augmentedMatrix(sz, 1);
    for (auto i = sz; i-- > 0;) {
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
