#include "git-rebase.h"
#include "git-fetch.h"
#include "git-stash.h"

#include <boost/process.hpp>


using namespace git;

int main(int argc, char* argv[]) {
    GitStashScope stash;
    {
        GitFetchScope fetch;
        silent = argc > 1;
    }

    rebase_remote_branches();
    rebase_local_branches();

    rebase("main");

    boost::process::child("\"" GIT_EXECUTABLE_PATH "\" gc");
    return 0;
}
