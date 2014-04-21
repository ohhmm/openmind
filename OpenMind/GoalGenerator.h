#pragma once
#include <vector>
#include <functional>
#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include "Goal.h"
#include "Named.h"
#include "Facility.h"

class Goal;

class GoalGenerator
	: public Named
{
	typedef Named			base_t;
	typedef GoalGenerator	self_t;

public:
	typedef boost::shared_ptr<self_t> ptr_t;
	typedef std::function<void (Goal::ptr_t)>  adder_t;

	virtual ~GoalGenerator(void);

	void Bind(adder_t adder);
    void Bind(Facility::ptr_t facility);
	void Unbind(adder_t adder);
    void PushGoal(Goal::ptr_t goal);

    void AddFacility(Facility::ptr_t facility);
    void UseFacilities(std::function<void (Facility::ptr_t)> f);

	virtual bool IsNeedToGenerate();
	virtual Goal::ptr_t GenerateGoal() = 0;
	typedef std::function<void()> need_generate_notification_fn_t;
	virtual void SubscribeGeneration(need_generate_notification_fn_t fn);

protected:
	std::vector<need_generate_notification_fn_t> needGenerationNotificators_;

	std::vector<adder_t>	        adders_;

	typedef std::vector<Facility::ptr_t>                facility_collection_t;
	typedef boost::shared_ptr<facility_collection_t>    facility_collection_ptr_t;
	facility_collection_ptr_t       facilities_;

	GoalGenerator(string_t::const_pointer name);

};
