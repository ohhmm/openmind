//
// Created by Сергей Кривонос on 02.08.17.
//

#pragma once

#include <algorithm>
#include <vector>

#include "Expression.h"
#include "Extrapolator.h"

class System
{
    using expressions_container = std::vector<Expression>;
    expressions_container expressions;

    auto BuildMatrix(const expressions_container& exprs, const Expression& expr) const
    {
        omnn::extrapolator::Extrapolator e;
        
        //TODO : build
        return e;
    }

    bool IsPossible(const Expression& e) const
    {
//        auto m = BuildMatrix(expressions, e);
//        return m.Consistent();
         throw "not implemented!";
        return true;
    }
public:
    operator Expression()
    {
        Expression result;
        for(auto& e : expressions)
        {
            result += e.Sqr();
        }
        return result;
    }

    void Add(const Expression& e)
    {
        if(!IsPossible(e))
            throw "Impossible";
        expressions.push_back(e);
    }
};

