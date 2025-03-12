#include "git-stash.h"

#include <boost/process.hpp>

#define GIT_STASH "\"" GIT_EXECUTABLE_PATH "\" stash"
#define GIT_STASH_POP "\"" GIT_EXECUTABLE_PATH "\" stash pop"


namespace git {

void GitStashScope::stash() {
	boost::process::child stash(GIT_STASH);
}

void GitStashScope::pop() {
	boost::process::child stash(GIT_STASH_POP);
}

GitStashScope::GitStashScope() {
	stash();
}

GitStashScope::~GitStashScope() {
	pop();
}

} // namespace git

// namespace git