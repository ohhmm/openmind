#pragma once
#include <functional>
#include <sstream>
#include <thread>
#include <memory>
#include "Facility.h"

class GoalGenerator;
class Facility;
class Goal;

class Goal
	: public std::enable_shared_from_this<Goal>
{
public:
	typedef PTRT(Goal)      	ptr_t;
	typedef std::list<ptr_t>	container_t;
	typedef std::wstring		string_t;

	Goal();
	virtual ~Goal(void);

    void SubscribeOnReach(std::function <void()> f);
    void OnReach();

	virtual bool		Reach() = 0;
	virtual container_t ToReach();
    virtual void		GetResult(void*) = 0;
	virtual void*		GetResult() = 0;
	virtual string_t	SerializedResult();
	virtual string_t	Name() = 0;
	virtual bool		IsReachable();
    virtual bool        Archivable() { return false; }

	void StartReachingAsync();
	bool CompleteReaching();

protected:
    typedef PTRT(GoalGenerator)    parent_generator_ptr_t;
	std::list<Facility>		_facilities;
	std::set<Goal::ptr_t>	_dependencies;
    parent_generator_ptr_t  _parentGenerator;
    std::wstringstream      _result;

    Goal(parent_generator_ptr_t  parentGenerator);

private:
    void* res_;
    std::vector<std::function <void()> > _onReach;
	std::thread _reaching;
};

#include "GoalGenerator.h"
