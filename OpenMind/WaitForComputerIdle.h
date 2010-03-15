#pragma once
#include <boost/function.hpp>
#include "Goal.h"
#include "Facility.h"

class WaitForComputerIdle :
	public Goal
{
	//boost::function<void()> command_;
    typedef boost::shared_ptr< std::vector<Facility::ptr_t> >       facility_collection_ptr_t;
    facility_collection_ptr_t   facilities_;
public:
    WaitForComputerIdle(facility_collection_ptr_t facilities);
	bool        Reach();
    void		GetResult(void*);
    string_t	SerializedResult();
    string_t	Name();

	~WaitForComputerIdle(void);
};
