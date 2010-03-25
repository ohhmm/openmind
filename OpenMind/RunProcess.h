#pragma once
#include <string>
#include "facility.h"

class RunProcess :
    public Facility
{
    PROCESS_INFORMATION procInfo_;
    DWORD creationFlags_;
    std::wstring cmd_;

    template <class StrT>
    RunProcess(const StrT& cmd)
        : cmd_(cmd),
        creationFlags_(DETACHED_PROCESS)
    {
        cmd_.reserve(MAX_PATH); // for be passed to CreateProcessW
    }

public:

    template <class StrT>
    static Facility::ptr_t Make(const StrT& cmd)
    {
        return Facility::ptr_t( new RunProcess(cmd) );
    }

    void SetNoWindow();
    bool Invoke();

    bool TryShutdown()
    {
        return false;
    }

    void ForceShutdown()
    {
        TerminateProcess(procInfo_.hProcess, 0);
    }
};
