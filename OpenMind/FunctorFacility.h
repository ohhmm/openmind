#pragma once
#include <functional>
#include "Facility.h"


class FunctorFacility :
    public Facility
{
    std::function<bool ()> _f;
public:
    FunctorFacility(std::function<bool ()> f);
    bool Invoke();
};
