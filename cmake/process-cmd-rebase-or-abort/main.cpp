#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/process.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>
#include <utility>

// defaults used for rebase-all-branches target
#define INPUT_FILE_DEFAULT SRC_DIR "rebase.cmd"
#define GIT_REBASE_ABORT "\"" GIT_EXECUTABLE_PATH "\" rebase --abort"
#define SUFFIX_DEFAULT " || " GIT_REBASE_ABORT


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <input_file> <suffix>" << std::endl;
        std::cout << "Defaults used: <input_file>=" INPUT_FILE_DEFAULT " <suffix>=" SUFFIX_DEFAULT << std::endl;
    }

    std::string_view input_file = argc == 3 ? argv[1] : INPUT_FILE_DEFAULT;
    std::string_view suffix = argc == 3 ? argv[2] : SUFFIX_DEFAULT;

    std::ifstream infile(input_file.data());
    if (!infile.is_open()) {
        std::cerr << "Failed to open file: " << input_file << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(infile, line)) {
        boost::process::child rebase(line);
        rebase.join();
        auto code = rebase.exit_code();
        std::cout << "exit code: " << code << ' ' << line << std::endl;

        if (code != 0) {
            boost::process::child abort(GIT_REBASE_ABORT);
            abort.join();
            code = abort.exit_code();
            std::cout << "exit code: " << code << ' ' << GIT_REBASE_ABORT << std::endl;

            if (code != 0) {
                std::cerr << "Failed to abort rebase" << std::endl;
                std::exit(1);
            }
        }
    }

    infile.close();
    return 0;
}
