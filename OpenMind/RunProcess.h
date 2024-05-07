#pragma once
#include <string>
#include "Facility.h"

class RunProcess: public Facility {
public:
    typedef std::string string_t;

    explicit RunProcess(const string_t& cmd);

    static Facility::ptr_t Make(const string_t& cmd);

	virtual void SetNoWindow();
	bool Invoke();

	virtual bool TryShutdown();

	virtual void ForceShutdown();

protected:
    string_t cmd_;
};
