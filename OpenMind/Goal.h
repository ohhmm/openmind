#pragma once
#include <boost/shared_ptr.hpp>
#include "Facility.h"

class Facility;
class Goal;

class Goal
{
public:
	typedef boost::shared_ptr<Goal>	ptr_t;
	typedef std::list<ptr_t>		container_t;
	typedef std::wstring			string_t;

	Goal(void);
	virtual ~Goal(void);

	virtual bool		Reach() = 0;
	virtual container_t ToReach();
	virtual void		GetResult(void*) = 0;
	virtual string_t	SerializedResult() = 0;
	virtual string_t	Name() = 0;

protected:
	std::list<Facility>		_facilities;
	std::set<Goal::ptr_t>	_dependencies;
};
