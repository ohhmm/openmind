#pragma once
#include <Windows.h>

class RiseEventFn
{
	HANDLE e_;
public:
	RiseEventFn(HANDLE e);
	void operator()();
};
