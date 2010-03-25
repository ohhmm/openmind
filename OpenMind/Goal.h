#pragma once
#include <sstream>
#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include "Facility.h"

class GoalGenerator;
class Facility;
class Goal;

class Goal
{
public:
	typedef PTRT(Goal)      	ptr_t;
	typedef std::list<ptr_t>	container_t;
	typedef std::wstring		string_t;

	Goal();
	virtual ~Goal(void);

    void SubscribeOnReach(boost::function <void()> f);
    void OnReach();

	virtual bool		Reach() = 0;
	virtual container_t ToReach();
    virtual void		GetResult(void*) = 0;
	virtual string_t	SerializedResult()
    {
        return _result.str();
    }
	virtual string_t	Name() = 0;
    virtual bool        Archivable() { return false; }

protected:
    typedef PTRT(GoalGenerator)    parent_generator_ptr_t;
	std::list<Facility>		_facilities;
	std::set<Goal::ptr_t>	_dependencies;
    parent_generator_ptr_t  _parentGenerator;
    std::wstringstream      _result;

    Goal(parent_generator_ptr_t  parentGenerator);

private:
    void* res_;
    std::vector<boost::function <void()> > _onReach;
};

#include "GoalGenerator.h"
