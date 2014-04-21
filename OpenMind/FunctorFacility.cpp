#include "stdafx.h"
#include "FunctorFacility.h"

FunctorFacility::FunctorFacility( std::function<bool ()> f ) : _f(f)
{

}

bool FunctorFacility::Invoke()
{
    return _f();
}
