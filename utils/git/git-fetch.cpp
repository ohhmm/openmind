#include "git-fetch.h"


#define GIT_FETCH_ALL "\"" GIT_EXECUTABLE_PATH "\" fetch --all --prune"


namespace git {

void fetch_start() {
	boost::process::child fetch(GIT_FETCH_ALL);
}

void fetch() {
	GitFetchScope();
}


GitFetchScope::GitFetchScope()
	: boost::process::child(GIT_FETCH_ALL)
{}

GitFetchScope::~GitFetchScope() {
	this->join();
}

} // namespace git