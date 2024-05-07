#include "SolveEquation.h"
#include "Variable.h"
#include "VarHost.h"

#include <sstream>


SolveEquation::SolveEquation( const std::wstring& equation )
: equation_(equation)
{
    using namespace omnn::math;
    std::string equation_str(equation.begin(), equation.end());
    std::string_view sv(equation_str);
    auto& varHost = VarHost::Global<std::string>();
    auto varHostPtr = varHost.sh();
    Valuable eq(sv, varHostPtr);
    for (auto&& solution : eq.Solutions())
        _result << solution << ' ';
}

SolveEquation::~SolveEquation(void)
{
}
