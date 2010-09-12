#include "StdAfx.h"
#include "CollectKnowledge.h"

CollectKnowledge::CollectKnowledge(void)
{
    
}

CollectKnowledge::~CollectKnowledge(void)
{
}

bool CollectKnowledge::Invoke()
{
    if (!thread_.joinable())
    {
        thread_.start_thread();
    }

    return true;
}