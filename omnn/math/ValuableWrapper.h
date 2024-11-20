#pragma once

#include "Valuable.h"
#include "ConcreteValuable.h"
#include <memory>
#include <ranges>
#include <string>
#include <string_view>
#include <map>
#include <set>

#ifdef TBB_PREVIEW_PARTITIONER_1_0
namespace tbb {
namespace detail {
namespace d1 {
    class static_partition_type {
    public:
        size_t my_divisor;
        size_t do_split(const static_partition_type& src, const proportional_split& split_obj) {
            return split_obj.right();
        }
    };

    class affinity_partition_type {
    public:
        size_t my_divisor;
        size_t do_split(const affinity_partition_type& src, const proportional_split& split_obj) {
            return split_obj.right();
        }
    };
}}}
#endif

namespace omnn::math {

class ValuableWrapper {
    std::shared_ptr<Valuable> value;

public:
    ValuableWrapper() noexcept = default;
    ValuableWrapper(std::shared_ptr<Valuable> v) noexcept : value(std::move(v)) {}
    ValuableWrapper(const ValuableWrapper&) = default;
    ValuableWrapper(ValuableWrapper&&) noexcept = default;
    ValuableWrapper& operator=(const ValuableWrapper&) = default;
    ValuableWrapper& operator=(ValuableWrapper&&) noexcept = default;
    ~ValuableWrapper() = default;
    // String constructors
    ValuableWrapper(const std::string& str, VarHost::ptr host, bool optimized = false)
        : value(std::make_shared<ConcreteValuable>(str, std::move(host), optimized)) {}

    ValuableWrapper(const std::string& str, const Valuable::va_names_t& vaNames, bool optimized = false)
        : value(std::make_shared<ConcreteValuable>(str, vaNames, optimized)) {}

    ValuableWrapper(std::string_view str, const Valuable::va_names_t& vaNames, bool optimized = false)
        : value(std::make_shared<ConcreteValuable>(str, vaNames, optimized)) {}

    // String conversion methods
    std::string str() const {
        return value ? value->str() : "";
    }

    std::string toString() const {
        return str();
    }

    // Access to VaNames
    Valuable::va_names_t VaNames() const {
        return value ? value->VaNames() : Valuable::va_names_t{};
    }

    // Optimization status
    bool is_optimized() const noexcept {
        return value ? value->is_optimized() : false;
    }



    std::shared_ptr<Valuable> get() const noexcept { return value; }
    operator std::shared_ptr<Valuable>() const noexcept { return value; }
    operator bool() const noexcept { return value != nullptr; }

    // Delegate methods to underlying Valuable object
    size_t Hash() const noexcept {
        return value ? value->Hash() : 0;
    }

    size_t getMaxVaExp() const noexcept {
        return value ? boost::numeric_cast<size_t>(value->getMaxVaExp()) : 0;
    }

    // Conversion operators
    operator Valuable&() {
        if (!value) throw std::runtime_error("Null valuable wrapper");
        return *value;
    }

    operator const Valuable&() const {
        if (!value) throw std::runtime_error("Null valuable wrapper");
        return *value;
    }

    // Delegate common Valuable methods
    bool Same(const Valuable& other) const noexcept {
        return value && value->Same(other);
    }

    bool IsMultival() const noexcept {
        return value ? value->IsMultival() : false;
    }

    ValuableWrapper Link() const noexcept {
        return value ? ValuableWrapper(value) : ValuableWrapper();
    }

    // Collection operation helpers
    template<typename Container>
    void emplace_back(const Valuable& v) {
        if (auto ptr = const_cast<Valuable&>(v).SharedFromThis()) {
            static_cast<Container&>(*this).emplace_back(std::move(ptr));
        }
    }

    std::shared_ptr<Valuable> SharedFromThis() const noexcept {
        return value;
    }

    // Additional Valuable interface methods
    bool HasVa(const Variable& va) const noexcept {
        return value && value->HasVa(va);
    }

    bool eval(const std::map<Variable, ValuableWrapper>& with) noexcept {
        if (!value) return false;
        std::map<Variable, Valuable> converted_map;
        for (const auto& [var, wrapper] : with) {
            if (wrapper.get()) {
                converted_map.emplace(var, *wrapper.get());
            }
        }
        return value->eval(converted_map);
    }

    void optimize() noexcept {
        if (value) value->optimize();
    }

    bool operator==(const ValuableWrapper& other) const noexcept {
        if (!value && !other.value) return true;
        if (!value || !other.value) return false;
        return *value == *other.value;
    }

    bool operator<(const ValuableWrapper& other) const noexcept {
        if (!value && !other.value) return false;
        if (!value) return true;
        if (!other.value) return false;
        return *value < *other.value;
    }
};

} // namespace omnn::math
