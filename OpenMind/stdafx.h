// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// STL
#include <algorithm>
#include <deque>
#include <iostream>
#include <set>
#include <string>
#include <list>
#include <map>
#include <vector>

// boost
#include <boost/bind.hpp>
#include <boost/detail/lightweight_mutex.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#define PTRT(T) boost::shared_ptr<T>
