#pragma once
#include <boost/shared_ptr.hpp>
#include "Facility.h"

class Facility;
class Goal;

class Goal
{
protected:
	std::list<Facility> usingFacilities;
public:
	typedef boost::shared_ptr<Goal>		ptr_t;
	typedef std::list<ptr_t>		container_t;
	virtual bool Rich();
	virtual container_t ToRich();
	Goal(void);
	virtual ~Goal(void);
};
