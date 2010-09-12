#pragma once
#include "facility.h"
#include <boost/thread.hpp>

class CollectKnowledge :
    public GenericFacility<CollectKnowledge>
{
    boost::thread thread_;

public:
    CollectKnowledge(void);
    ~CollectKnowledge(void);

    //bool Use(goal_ptr_t goal);
    //std::list<goal_ptr_t> ToUse();
    bool Invoke();

    //bool TryShutdown();
    //void ForceShutdown();
};
