#include "StdAfx.h"
#include "RunProcess.h"


void RunProcess::SetNoWindow()
{
    creationFlags_ |= CREATE_NO_WINDOW;
}

bool RunProcess::Invoke()
{
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    return !! CreateProcessW(0, const_cast<wchar_t*>(cmd_.c_str()),
        0, 0, 0, creationFlags_, 0,0, &si, &procInfo_);
}

bool RunProcess::TryShutdown()
{
    return false;
}

void RunProcess::ForceShutdown()
{
    TerminateProcess(procInfo_.hProcess, 0);
}