#include "stdafx.h"
#include "GeneratorProxy.h"
#include "ConsoleGeneratorProxy.h"

GeneratorProxy::GeneratorProxy(string_t::pointer name)
: base_t(name)
{
}

GeneratorProxy::~GeneratorProxy(void)
{
}

GoalGenerator::ptr_t GeneratorProxy::Make()
{
	// TODO : Detect platform and use corresponding proxy
	return GoalGenerator::ptr_t(
		new ConsoleGeneratorProxy()
		);
}