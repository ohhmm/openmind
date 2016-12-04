#include "stdafx.h"
#include "GeneratorProxy.h"
#include "ConsoleGeneratorProxy.h"

GeneratorProxy::GeneratorProxy(string_t::const_pointer name)
: base_t(name)
{
}

GeneratorProxy::~GeneratorProxy(void)
{
}

GoalGenerator::ptr_t GeneratorProxy::Make(const goal_generator_collection_t& goalGeneratorCollection)
{
	// TODO : Detect platform and use corresponding proxy
	return GoalGenerator::ptr_t(
		new ConsoleGeneratorProxy(goalGeneratorCollection)
		);
}
