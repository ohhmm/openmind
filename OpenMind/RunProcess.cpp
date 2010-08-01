#include "stdafx.h"
#include "RunProcess.h"

#include <cstring>



void RunProcess::SetNoWindow() {
}

bool RunProcess::Invoke() {
	return false;
}

bool RunProcess::TryShutdown() {
	return false;
}

void RunProcess::ForceShutdown() {
}

RunProcess::RunProcess(const string_t& cmd) :
		cmd_(cmd) {
}



#ifdef _WIN32
class RunWinProcess : public RunProcess {
	PROCESS_INFORMATION procInfo_;
	DWORD creationFlags_;

public:
	using RunProcess::RunProcess;

    RunWinProcess(const string_t& cmd)
	: RunProcess(cmd)
	, creationFlags_(DETACHED_PROCESS)
	{
		cmd_.reserve(MAX_PATH); // for be passed to CreateProcessW
	}

	void SetNoWindow()
	{
		creationFlags_ |= CREATE_NO_WINDOW;
	}

	bool Invoke()
	{
		STARTUPINFOA si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		return !! CreateProcessA(0, 
				const_cast<LPSTR>(cmd_.c_str()), // we reserved MAX_PATH in constructor
				0, 0, 0, creationFlags_, 0,0, &si, &procInfo_);
	}

	void ForceShutdown()
	{
		TerminateProcess(procInfo_.hProcess, 0);
	}

};

#else
#include <spawn.h>
#include <signal.h>

class RunPosixProcess: public RunProcess {
	pid_t pid;
public:
	using RunProcess::RunProcess;
    
	void SetNoWindow() {

	}

	bool Invoke() {
        string_t::const_pointer argv[] = { cmd_.c_str(), (string_t::const_pointer) 0 };
		int status;
		fflush(NULL);
		status = posix_spawn(&pid, cmd_.c_str(), nullptr, nullptr, nullptr, nullptr);
		if (status == 0) {
			printf("Child id: %i\n", pid);
			fflush(NULL);
		} else {
			printf("posix_spawn: %s\n", strerror(status));
		}

		return status == 0;
	}

	bool TryShutdown() {
		return 0 == kill(pid, SIGTERM);
	}

	void ForceShutdown() {
		kill(pid, SIGKILL);
	}

};
#endif

Facility::ptr_t RunProcess::Make(const string_t& cmd) {
	return Facility::ptr_t(
#ifdef _WIN32
	new RunWinProcess(cmd)
#else
	new RunPosixProcess(cmd)
#endif
	);
}

