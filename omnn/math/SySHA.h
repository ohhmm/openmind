/*
 * SySHA.h
 *
 *  Created on: May 12, 2021
 *      Author: Serg
 */

#pragma once

#include "System.h"

namespace omnn::math {

class SySHA
{
    Variable v[256];
    Valuable b[256];
    System s;

public:
    template <class F>
    SySHA& operator<<(F&& f) {
        IMPLEMENT
    }
};

}
