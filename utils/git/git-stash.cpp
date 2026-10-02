#include "git-stash.h"

#if __has_include(<boost/process/v1.hpp>)
#include <boost/process/v1.hpp>
namespace bp = boost::process::v1;
#else
#include <boost/process.hpp>
namespace bp = boost::process;
#endif

#define GIT_STASH "\"" GIT_EXECUTABLE_PATH "\" stash"
#define GIT_STASH_POP "\"" GIT_EXECUTABLE_PATH "\" stash pop"


namespace git {

void GitStashScope::stash() {
	bp::child stash(GIT_STASH);
}

void GitStashScope::pop() {
	bp::child stash(GIT_STASH_POP);
}

GitStashScope::GitStashScope() {
	stash();
}

GitStashScope::~GitStashScope() {
	pop();
}

} // namespace git

// namespace git