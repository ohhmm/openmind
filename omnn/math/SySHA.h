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
        : protected System
{
    Valuable b[256];
public:
    template <class F>
    SySHA& operator<<(F&& f) {
        IMPLEMENT
    }
};

}
