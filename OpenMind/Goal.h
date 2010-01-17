#pragma once
#include <boost/shared_ptr.hpp>
#include "Facility.h"

class Facility;
class Goal;

class Goal
{
public:
	typedef boost::shared_ptr<Goal>		ptr_t;
	typedef std::list<ptr_t>		container_t;
	virtual bool Rich();
	virtual container_t ToRich();
	virtual void GetResult(void*) = 0;
	Goal(void);
	virtual ~Goal(void);
protected:
	std::list<Facility>		usingFacilities;
	std::set<Goal::ptr_t>	_dependencies;
};
