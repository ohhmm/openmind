
#include "gen.h"

#include <omnn/math/Variable.h>
using namespace omnn::math;

#include <boost/archive/binary_oarchive.hpp>
#ifdef OPENMIND_USE_OPENCL
#include <boost/compute.hpp>
#include <boost/compute/core.hpp>
#endif
#include <boost/lambda2.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/program_options.hpp>

#include <fstream>
#include <boost/serialization/vector.hpp>

uint64_t N = 0;

auto BuildFormula() {

    std::cout << "For i=[0;" << N << ") enter pattern f(i):" << std::endl;
    std::string s;
    std::cin >> s;
//    s="i*i*i + -1*i +-1*i";
	std::cout << std::endl;
    return gen::BuildFormula(s, N);
}



int main()
{
    std::cout << "N=";
    std::cin >> N;
    std::cout << std::endl;

    auto y = BuildFormula();
    std::cout << "Y = " << y << std::endl;
    auto openCLcode = y.OpenCL();
    std::cout << "\nGenerate OpenCL code:\n" << openCLcode << std::endl;

	std::vector<float_t> Y(N);
	gen::Generate(y, &Y[0], N);



    std::cout << "Result[" << Y.size() << "]:\n";
    for(auto& item: Y)
        std::cout << ' ' << item;
    std::cout << std::endl;

    std::cout << "Save the data?"  << std::endl;
    std::string s;
    std::cin >> s;
    if(s.size()){
        char c = s[0];
        if(c == 'y' || c=='Y'){
            std::cout << "Please, enter filename"  << std::endl;
            std::cin >> s;
            // serialize vector
            {
                std::ofstream ofs(s);
                boost::archive::binary_oarchive oa(ofs);
                oa & Y;
            }
            std::cout << std::endl;
        }
    }
    return 0;
}
