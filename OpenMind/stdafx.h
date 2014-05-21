// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WIN32
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#endif

// C RunTime Header Files
//#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
#include <assert.h>

#ifdef _WIN32
#include <tchar.h>
#endif

// STL
#include <algorithm>
#include <deque>
#include <iostream>
#include <set>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <mutex>
#include <functional>
#include <chrono>
#include <thread>

#define PTRT(T) std::shared_ptr<T>
