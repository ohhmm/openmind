#pragma once
#include <boost/function.hpp>
#include "facility.h"


class FunctorFacility :
    public Facility
{
    boost::function<bool ()> _f;
public:
    FunctorFacility(boost::function<bool ()> f);
    bool Invoke();
};
