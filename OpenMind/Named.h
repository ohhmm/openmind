#pragma once
#include <string>

class Named
{
public:
	typedef std::wstring  string_t;

	string_t Name() const { return _name; }
	//void Name(string_t val) { _name = val; }

#ifndef _DEBUG
	Named();
#endif

	template<class NameT>
	Named(const NameT& name) : _name(name) {}

	virtual ~Named(void) {}

private:
	string_t _name;
};
