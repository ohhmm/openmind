#include <fstream>
#include <iostream>
#include <string>

// defaults used for rebase-all-branches target
#define INPUT_FILE_DEFAULT  SRC_DIR "rebase.cmd"
#define SUFFIX_DEFAULT  " || \"" GIT_EXECUTABLE_PATH "\" rebase --abort"

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

    std::string output_file = input_file.data();
    output_file += ".new";

    std::ofstream outfile(output_file);
    if (!outfile.is_open()) {
        std::cerr << "Failed to create file: " << output_file << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(infile, line)) {
        outfile << line << suffix << std::endl;
    }

    infile.close();
    outfile.close();

    std::remove(input_file.data());
    std::rename(output_file.c_str(), input_file.data());

    return 0;
}
