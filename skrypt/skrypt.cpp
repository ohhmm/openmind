

#include "skrypt.h"

#include <omnn/math/Variable.h>
using namespace omnn::math;

#include <boost/serialization/vector.hpp>
#include <boost/program_options.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/lambda2.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <iostream>
#include <vector>


namespace {
	bool IsInteractiveMode = true;
	bool Echo = true;
    boost::program_options::options_description Options("Options");
    std::vector<boost::filesystem::path> filepath;
    std::vector<boost::filesystem::path> outputs;
	std::vector<boost::filesystem::path> genCppPath;
	std::vector<boost::filesystem::path> saveSystemPath;
    auto& desc = Options.add_options()
		("help", "produce help message")
		("echo", boost::program_options::value(&Echo)->default_value(Echo), "Load file/pipe path or 'stdin'")
		("load,file", boost::program_options::value(&filepath), "Load file/pipe path or 'stdin'")
		("o,output", boost::program_options::value(&outputs), "Output file/pipe path or 'stdout'")
		("i,interactive,stdin", boost::program_options::value(&IsInteractiveMode)->default_value(true), "Continue interactive mode after loading scripts")
		("gen-opencl-kernel", boost::program_options::value(&genCppPath), "Generate OpenCL compute kernel code")
		("gen-cpp", boost::program_options::value(&genCppPath), "Generate C++ code")
		("save-system", boost::program_options::value(&saveSystemPath), "Save system binary file/pipe paths or 'stdout'")
		;
    boost::program_options::variables_map vm;
}
int main(int argc, const char* const argv[])
{
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, Options), vm);
	boost::program_options::notify(vm);
	if (vm.count("help")) {
		std::cout << "Usage: options_description [options]\n" << Options;
		exit(0);
	}

	std::cout << "SKRYPT RULEZ:\n"
		"Accepts three forms of input :\n"
		"- equations with any variables are automatically equaled to zero\n"
		"- single variable name to show its value if known\n"
		"- expression with question mark to check if its true\n";
    if (argc > 1)
    {
        skrypt::Skrypt s;
        for (auto& o : outputs) {
            if (o == "stdout") {
                Echo = {};
                s.BindTargetStream(std::cout);
            } else {
                s.BindTargetStream(o);
            }
        }
        s.Echo(Echo || IsInteractiveMode);
		//s.MakesTotalEqu(true);
		if (filepath.size() == 0 && argc == 2) {
			filepath.push_back(argv[1]);
		}
		for(auto& file : filepath)
			s.Load(file);
		if(IsInteractiveMode)
			s.Load(std::cin);
    }
    else {
        skrypt::Skrypt s(std::cin);
    }
    return 0;
}

