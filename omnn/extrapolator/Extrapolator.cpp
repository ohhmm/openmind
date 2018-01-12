//
// Created by Сергей Кривонос on 10.08.17.
//

#include "Extrapolator.h"
#include "Integer.h"

using namespace omnn;
using namespace extrapolator;


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
    Product e;
    auto szy = size1();
    auto szx = size2();
    auto optsWas = Valuable::optimizations;
    Valuable::optimizations = {};
    for (auto y = 0; y < szy; ++y) {
        for (auto x = 0; x < szx; ++x) {
            e.Add(((row-y)^2)
                  +((col-x)^2)
                  +((val-(*this)(y,x))^2));
        }
    }
    Valuable::optimizations = optsWas;
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
        auto& v = vm(i,2);
        integers = integers && Integer::cast(v);
        auto e1 = vx - vm(i,0);
        auto e2 = vy - vm(i,1);
        auto e3 = vv - v;
        auto subsyst = e1*e1 + e2*e2 + e3*e3;
        e *= subsyst;
    }
    Valuable::optimizations = true;
    e.optimize();
    auto s = Sum::cast(e);
    if(!s)
        throw "Debug!";
    if(integers)
        return FormulaOfVaWithSingleIntegerRoot(vv, *s);
    else
        return s->FormulaOfVa(vv);
}

