/*
 * Pattern.cpp
 *
 * Created on: April 29, 2024
 * Author: devin
 */

#include "Pattern.h"
#include <algorithm>
#include <regex>

namespace omnn {
namespace math {

Pattern::Pattern(const std::string_view& str, std::shared_ptr<VarHost> host, bool itIsOptimized)
{
    std::regex pattern(R"(Function\(\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*,\s*\[(.*?)\]\s*,\s*(.*)\))");
    std::cmatch match;
    
    if (std::regex_match(str.begin(), str.end(), match, pattern)) {
        name = match[1].str();
        
        std::string argsStr = match[2].str();
        std::regex argsPattern(R"(\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*(?:,|$))");
        std::cregex_iterator argsBegin(argsStr.c_str(), argsStr.c_str() + argsStr.size(), argsPattern);
        std::cregex_iterator argsEnd;
        
        for (auto it = argsBegin; it != argsEnd; ++it) {
            std::cmatch argMatch = *it;
            std::string argName = argMatch[1].str();
            arguments.push_back(Variable(argName, host));
        }
        
        std::string exprStr = match[3].str();
        expression = Valuable(exprStr, host);
    } else {
        LOG_AND_IMPLEMENT("Invalid Pattern format: " << str);
    }
    
    hash = std::hash<std::string>()(name);
    for (const auto& arg : arguments) {
        hash ^= arg.Hash();
    }
    hash ^= expression.Hash();
    
    if (itIsOptimized) {
        optimized = true;
    }
}

Valuable Pattern::operator()(const std::vector<Valuable>& args) const
{
    if (args.size() != arguments.size()) {
        LOG_AND_IMPLEMENT("Function call with wrong number of arguments: " << args.size() << " vs " << arguments.size());
        return *this;
    }
    
    std::map<Variable, Valuable> substitutions;
    for (size_t i = 0; i < arguments.size(); ++i) {
        substitutions[arguments[i]] = args[i];
    }
    
    Valuable result = expression;
    result.eval(substitutions);
    return result;
}

Valuable::universal_lambda_t Pattern::CompileIntoLambda(Valuable::variables_for_lambda_t vars) const
{
    return [this, argLambdas = std::vector<Valuable::universal_lambda_t>()](Valuable::universal_lambda_params_t params) -> Valuable {
        return *this;
    };
}

} /* namespace math */
} /* namespace omnn */
