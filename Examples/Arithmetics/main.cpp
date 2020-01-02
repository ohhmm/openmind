// OpenMind.cpp : Defines the entry point for the application.
//

#include <OpenMind/Mind.h>
#include <OpenMind/ConsoleGeneratorProxy.h>
#include <OpenMind/IdleTimeGoalGenerator.h>
#include <OpenMind/RunProcess.h>
#include "ProportionGenerator.h"
#include "GetEquation.h"

#ifdef _WIN_32
#include "resource.h"
#endif

int main()
{
	auto goalGenerator = ConsoleGeneratorProxy::Make( { GetEquation::Make() } );
    goalGenerator->AddFacility(RunProcess::Make(
#ifndef _WIN32
    		"k"
#endif
    		"calc"
    		));

	Mind mind;
	mind.AddGoalGenerator(goalGenerator);
	mind.Run();
    return 0;
}
