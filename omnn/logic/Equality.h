//
// Created by Sergej Krivonos on 16.01.18.
//

#pragma once
#include "math/ValuableDescendantContract.h"

namespace omnn{
namespace logic {
// if a==1 then b==1 else b==-1
    // A: a==1
    // B: b==1
    //   E: (((a-1)^2)+((b-1)^2))(b+1)==0
    // !A:  E && b==-1  :  E^2+(b+1)^2=0  :
    // Total:  (A && B) || !A && b==-1  :  ((a-1)^2+(b-1)^2)(((((a-1)^2)+((b-1)^2))(b+1))^2+(b+1)^2)

class Equality {

};

}}
