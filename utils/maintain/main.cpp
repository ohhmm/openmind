#include "git-rebase.h"
#include "git-fetch.h"
#include "git-stash.h"

#if __has_include(<boost/process/v1.hpp>)
#include <boost/process/v1.hpp>
namespace bp = boost::process::v1;
#else
#include <boost/process.hpp>
namespace bp = boost::process;
#endif


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

    bp::child("\"" GIT_EXECUTABLE_PATH "\" gc");
    return 0;
}
