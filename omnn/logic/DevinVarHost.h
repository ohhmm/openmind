#pragma once
#include <omnn/math/Platform.h>

#include <omnn/math/Integer.h>
#include <any>
#include <map>
#include <string>

namespace omnn {
namespace math {
class VarHost;
class Variable;
} // namespace math
namespace logic {

class DevinVarHost : public math::VarHost {
    std::map<std::string, math::Variable> variables;
    std::map<std::string, hosted_storage_t> storage;
    
public:
    const hosted_storage_t& HostedStorage(const std::any& id) override {
        auto& str = std::any_cast<const std::string&>(id);
        return storage[str];
    }
    
    bool IsIntegerId() const override { return false; }
    
    bool Has(const std::any& id) const override {
        try {
            auto& str = std::any_cast<const std::string&>(id);
            return variables.find(str) != variables.end();
        } catch (const std::bad_any_cast&) {
            return false;
        }
    }
    
    size_t Hash(const std::any& id) const override {
        return std::hash<std::string>{}(std::any_cast<const std::string&>(id));
    }
    
    std::any NewVarId() override {
        std::string newId = "var_" + std::to_string(variables.size());
        return std::make_any<std::string>(newId);
    }
    
    std::any CloneId(const std::any& a) override {
        return a;
    }
    
    bool CompareIdsLess(const std::any& a, const std::any& b) const override {
        return std::any_cast<const std::string&>(a) < std::any_cast<const std::string&>(b);
    }
    
    bool CompareIdsEqual(const std::any& a, const std::any& b) const override {
        return std::any_cast<const std::string&>(a) == std::any_cast<const std::string&>(b);
    }
    
    const math::Variable& Host(const std::any& id) override {
        auto& str = std::any_cast<const std::string&>(id);
        auto it = variables.find(str);
        if (it == variables.end()) {
            auto [inserted, _] = variables.emplace(str, math::Variable(shared_from_this()));
            return inserted->second;
        }
        return it->second;
    }
    
    math::Integer Stored() const override {
        return math::Integer(variables.size());
    }
};

}} // namespace omnn::logic
