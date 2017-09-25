//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <memory>
#include "Valuable.h"
#Include "VarHost.h"

namespace omnn{
namespace extrapolator {

class Variable
        : public ValuableDescentContract<Variable>
{
    std::shared_ptr<Varhost> varSetHost;
public:
};


}}