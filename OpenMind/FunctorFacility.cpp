#include "stdafx.h"
#include "FunctorFacility.h"

FunctorFacility::FunctorFacility( boost::function<bool ()> f ) : _f(f)
{

}

bool FunctorFacility::Invoke()
{
    return _f();
}
