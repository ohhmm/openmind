
#include "gen.h"
#include "fs_io.hpp"

#include <omnn/math/Variable.h>
#include <omnn/math/VarHost.h>
using namespace omnn::math;

#include <boost/archive/binary_oarchive.hpp>
#ifdef OPENMIND_USE_OPENCL
#include <boost/compute.hpp>
#include <boost/compute/core.hpp>
#endif
#include <boost/filesystem.hpp>
#include <boost/gil.hpp>

#include <boost/lambda2.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/program_options.hpp>

#include <string>


namespace {
	std::string StdIn(std::string_view name) {
		std::string str;
		std::cout << ' ' << name << '=';
		do { std::getline(std::cin, str); } while (str.empty());
		return str;
	}
}
int main(int argc, char** argv)
{
	using namespace std::string_view_literals;

    auto& programOptionValues = gen::Init(argc, argv);
	std::cout << "BG: " << programOptionValues.color << std::endl;

	auto width = 0, height = 0;
	std::cout << "Image size (pixels):" << std::endl
		<< " width="; std::cin >> width;
	std::cout << " height="; std::cin >> height;
	std::cout << std::endl;

	std::cout << "Image pattern (formulas for channels of width, height, x, y):" << std::endl;
	auto names = gen::InitialVarNames();
	auto less = "x"_va.IntMod_Less("y"_va);
	std::cout << "X<Y: " << less << std::endl;
	std::cout << "bool X<Y: ..."
		//<< less.ToBool()
		<< std::endl;
	auto varhost = names.begin()->second.getVaHost();
	Valuable red(StdIn("Red(w,h,x,y)"), varhost, true);
	Valuable green(StdIn("Green(w,h,x,y)"), varhost, true);
	Valuable blue(StdIn("Blue(w,h,x,y)"), varhost, true);
    std::cout << std::endl;

	auto img = gen::GeneratePatternImage(width, height, red, green, blue);
	if(gen::io::fs::Save(img, programOptionValues.filepath)){
		std::cout << programOptionValues.filepath << " is written" << std::endl;
	} else {
		std::cout << programOptionValues.filepath << " is not written" << std::endl;
	}

    return 0;
}
