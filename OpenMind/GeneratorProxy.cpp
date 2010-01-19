#include "StdAfx.h"
#include "GeneratorProxy.h"
#include "ConsoleGeneratorProxy.h"

GeneratorProxy::GeneratorProxy(void)
{
}

GeneratorProxy::~GeneratorProxy(void)
{
}

GeneratorProxy::ptr_t GeneratorProxy::Make()
{
	// TODO : Detect platform and use corresponding proxy
	return ptr_t(new ConsoleGeneratorProxy());
}