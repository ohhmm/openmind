#pragma once

class GeneratorProxy
{
public:
	typedef boost::shared_ptr<GeneratorProxy> ptr_t;
	static ptr_t Make();
protected:
	GeneratorProxy(void);
	virtual ~GeneratorProxy(void);
};
