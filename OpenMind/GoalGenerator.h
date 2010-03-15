#pragma once
#include <vector>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include "Goal.h"
#include "Named.h"
#include "Facility.h"

class GoalGenerator
	: public Named
{
	typedef Named			base_t;
	typedef GoalGenerator	self_t;

protected:
    typedef boost::function<void (Goal::ptr_t)>  adder_t;
    std::vector<adder_t>	        adders_;

    typedef std::vector<Facility::ptr_t>                facility_collection_t;
    typedef boost::shared_ptr<facility_collection_t>    facility_collection_ptr_t;
    facility_collection_ptr_t       facilities_;

	GoalGenerator(string_t::const_pointer name);

public:
	typedef boost::shared_ptr<self_t> ptr_t;

	virtual ~GoalGenerator(void);

	void Bind(adder_t adder);
    void Bind(Facility::ptr_t facility);
	void Unbind(adder_t adder);

	virtual bool IsNeedToGenerate();
	virtual Goal::ptr_t GenerateGoal() = 0;
};
