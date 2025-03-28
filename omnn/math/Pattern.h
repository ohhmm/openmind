/*
 * Pattern.h
 *
 * Created on: April 29, 2024
 * Author: devin
 */

#pragma once
#include <omnn/math/ValuableDescendantContract.h>
#include <omnn/math/Variable.h>
#include <omnn/math/VarHost.h>

#include <string>
#include <vector>

namespace omnn {
namespace math {

class Pattern
    : public ValuableDescendantContract<Pattern>
{
    using base = ValuableDescendantContract<Pattern>;
    
protected:
    std::string name;
    std::vector<Variable> arguments;
    Valuable expression;
    
    bool IsSubObject(const Valuable& o) const override {
        return this == &o.get() || expression == o;
    }
    
    std::ostream& print(std::ostream& out) const override {
        out << name << '(';
        for (size_t i = 0; i < arguments.size(); ++i) {
            if (i > 0) {
                out << ", ";
            }
            arguments[i].print(out);
        }
        out << ") = ";
        expression.print(out);
        return out;
    }
    
public:
    std::ostream& code(std::ostream& out) const override {
        out << name << '(';
        for (size_t i = 0; i < arguments.size(); ++i) {
            if (i > 0) {
                out << ", ";
            }
            arguments[i].code(out);
        }
        out << ") = ";
        expression.code(out);
        return out;
    }
    
    Pattern() = default;
    
    Pattern(const std::string& functionName, 
            const std::vector<Variable>& args, 
            const Valuable& expr)
        : name(functionName)
        , arguments(args)
        , expression(expr)
    {
        hash = std::hash<std::string>()(name);
        for (const auto& arg : arguments) {
            hash ^= arg.Hash();
        }
        hash ^= expression.Hash();
    }
    
    Pattern(const std::string_view& str, std::shared_ptr<VarHost> host = {}, bool itIsOptimized = {});
    
    const std::string& GetName() const { return name; }
    const std::vector<Variable>& GetArguments() const { return arguments; }
    const Valuable& GetExpression() const { return expression; }
    
    Valuable operator()(const std::vector<Valuable>& args) const;
    
    bool operator==(const Pattern& other) const {
        return hash == other.hash 
            && name == other.name 
            && arguments == other.arguments 
            && expression == other.expression;
    }
    
    bool operator==(const Valuable& other) const override {
        return other.Is<Pattern>() && operator==(other.as<Pattern>());
    }
    
    bool Same(const Valuable& other) const override {
        return other.Is<Pattern>() && hash == other.hash 
            && name == other.as<Pattern>().name 
            && arguments == other.as<Pattern>().arguments 
            && expression.Same(other.as<Pattern>().expression);
    }
    
    const Variable* FindVa() const override {
        return expression.FindVa();
    }
    
    bool HasVa(const Variable& va) const override {
        return expression.HasVa(va);
    }
    
    void CollectVa(std::set<Variable>& s) const override {
        expression.CollectVa(s);
    }
    
    void CollectVaNames(Valuable::va_names_t& s) const override {
        expression.CollectVaNames(s);
    }
    
    bool eval(const std::map<Variable, Valuable>& with) override {
        auto evaluated = expression.eval(with);
        if(evaluated) {
            hash = std::hash<std::string>()(name);
            for (const auto& arg : arguments) {
                hash ^= arg.Hash();
            }
            hash ^= expression.Hash();
        }
        return evaluated;
    }
    
    void Eval(const Variable& va, const Valuable& v) override {
        expression.Eval(va, v);
        hash = std::hash<std::string>()(name);
        for (const auto& arg : arguments) {
            hash ^= arg.Hash();
        }
        hash ^= expression.Hash();
    }
    
    Valuable::universal_lambda_t CompileIntoLambda(Valuable::variables_for_lambda_t vars) const override;
    
    static constexpr auto GetBinaryOperationLambdaTemplate() {
        return [](const auto& _1st, const auto& _2nd) { 
            LOG_AND_IMPLEMENT("Pattern does not support binary operations");
            return _1st; 
        };
    }
    
    void optimize() override {
        if (!optimizations) {
            return;
        }
        if (optimized) {
            return;
        }
        expression.optimize();
        optimized = true;
    }
    
    YesNoMaybe IsRational() const override {
        return expression.IsRational();
    }
    
    bool IsComesBefore(const Valuable& v) const override {
        if (v.Is<Pattern>()) {
            const auto& p = v.as<Pattern>();
            return name < p.name || 
                  (name == p.name && 
                   (arguments.size() < p.arguments.size() || 
                    (arguments.size() == p.arguments.size() && expression < p.expression)));
        }
        return base::IsComesBefore(v);
    }
};

} /* namespace math */
} /* namespace omnn */
