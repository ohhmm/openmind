#include "SolveEquation.h"
#include "Variable.h"
#include "VarHost.h"

#include <sstream>


SolveEquation::SolveEquation( const std::wstring& equation )
: equation_(equation)
{
    using namespace omnn::math;
    std::string s(equation.begin(), equation.end());
    std::string_view sv(s);
    auto& varHost = VarHost::Global<std::string>();
    auto varHostPtr = varHost.sh();
    Valuable eq(sv, varHostPtr);
    for (auto&& solution : eq.Solutions())
        _result << solution << ' ';
}

SolveEquation::~SolveEquation(void)
{
}
