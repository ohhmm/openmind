#include "git-fetch.h"


#define GIT_FETCH_ALL "\"" GIT_EXECUTABLE_PATH "\" fetch --all --prune"


namespace git {

void fetch_start() {
	bp::child fetch(GIT_FETCH_ALL);
}

void fetch() {
	GitFetchScope();
}


GitFetchScope::GitFetchScope()
	: bp::child(GIT_FETCH_ALL)
{}

GitFetchScope::~GitFetchScope() {
	this->join();
}

} // namespace git