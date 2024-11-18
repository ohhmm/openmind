#include "ConcreteValuable.h"
#include <stdexcept>
#include <typeinfo>

namespace omnn::math {

ConcreteValuable::ConcreteValuable(const std::string& str, VarHost::ptr host, bool optimized)
    : Valuable(ValuableDescendantMarker{}), expression_(str), host_(host), optimized_(optimized), sz(sizeof(ConcreteValuable)) {
    // Parse string and initialize valuable
    if (str.empty()) {
        throw std::invalid_argument("Empty string not allowed");
    }
    // TODO: Implement string parsing and initialization
    if (optimized) {
        optimize();
    }
}

ConcreteValuable::ConcreteValuable(const std::string& str, const Valuable::va_names_t& vaNames, bool optimized)
    : Valuable(ValuableDescendantMarker{}), expression_(str), optimized_(optimized), sz(sizeof(ConcreteValuable)) {
    // Parse string with variable names
    if (str.empty()) {
        throw std::invalid_argument("Empty string not allowed");
    }
    // TODO: Implement string parsing with variable names
    if (optimized) {
        optimize();
    }
}

ConcreteValuable::ConcreteValuable(std::string_view str, const Valuable::va_names_t& vaNames, bool optimized)
    : ConcreteValuable(std::string(str), vaNames, optimized) {
}

std::shared_ptr<Valuable> ConcreteValuable::Clone() const noexcept {
    return std::make_shared<ConcreteValuable>(*this);
}

std::shared_ptr<Valuable> ConcreteValuable::Move() noexcept {
    return std::make_shared<ConcreteValuable>(std::move(*this));
}

void ConcreteValuable::New(void* p, Valuable&& v) noexcept {
    new (p) ConcreteValuable(std::move(dynamic_cast<ConcreteValuable&>(v)));
}

size_t ConcreteValuable::getTypeSize() const noexcept {
    return sizeof(ConcreteValuable);
}

size_t ConcreteValuable::getAllocSize() const noexcept {
    return sz;
}

void ConcreteValuable::setAllocSize(size_t sz_) noexcept {
    sz = sz_;
}

bool ConcreteValuable::IsSubObject(const Valuable& o) const noexcept {
    return false;  // Default implementation
}

bool ConcreteValuable::HasVa(const Variable& va) const noexcept {
    return false;  // Default implementation
}

std::ostream& ConcreteValuable::print(std::ostream& out) const noexcept {
    return out << expression_;
}

std::wostream& ConcreteValuable::print(std::wostream& out) const noexcept {
    return out << std::wstring(expression_.begin(), expression_.end());
}

Valuable::encapsulated_instance ConcreteValuable::SharedFromThis() noexcept {
    return shared_from_this();
}

std::type_index ConcreteValuable::Type() const noexcept {
    return std::type_index(typeid(*this));
}

ValuableWrapper ConcreteValuable::Univariate() const noexcept {
    return ValuableWrapper(*this);  // Default implementation
}

ValuableWrapper ConcreteValuable::InCommonWith(const Valuable& v) const noexcept {
    return ValuableWrapper(*this);  // Default implementation
}

bool ConcreteValuable::IsVa() const noexcept {
    return false;
}

void ConcreteValuable::optimize() {
    optimized_ = true;
}

YesNoMaybe ConcreteValuable::IsMultival() const noexcept {
    return YesNoMaybe::No;
}

bool ConcreteValuable::IsSimple() const noexcept {
    return true;
}

bool ConcreteValuable::IsComesBefore(const Valuable& v) const noexcept {
    return expression_ < dynamic_cast<const ConcreteValuable&>(v).expression_;
}

a_int ConcreteValuable::Complexity() const noexcept {
    return expression_.length();
}

const Variable* ConcreteValuable::FindVa() const noexcept {
    return nullptr;  // Default implementation
}

void ConcreteValuable::CollectVa(::std::set<Variable>& s) const noexcept {
    // Default implementation
}

void ConcreteValuable::CollectVaNames(va_names_t& s) const noexcept {
    // Default implementation
}

bool ConcreteValuable::eval(const ::std::map<Variable, ValuableWrapper>& with) noexcept {
    return false;  // Default implementation
}

void ConcreteValuable::Eval(const Variable& va, const Valuable& v) noexcept {
    // Default implementation
}

void ConcreteValuable::solve(const Variable& va, solutions_t& solutions) const noexcept {
    // Default implementation
}

const Valuable::vars_cont_t& ConcreteValuable::getCommonVars() const noexcept {
    return common_vars_;
}

Valuable::vars_cont_t ConcreteValuable::GetVaExps() const noexcept {
    return vars_cont_t{};  // Default implementation
}

Valuable& ConcreteValuable::operator=(Valuable&& v) noexcept {
    if (this != &v) {
        auto& other = dynamic_cast<ConcreteValuable&>(v);
        expression_ = std::move(other.expression_);
        host_ = std::move(other.host_);
        optimized_ = other.optimized_;
        sz = other.sz;
    }
    return *this;
}

Valuable& ConcreteValuable::operator=(const Valuable& v) noexcept {
    if (this != &v) {
        auto& other = dynamic_cast<const ConcreteValuable&>(v);
        expression_ = other.expression_;
        host_ = other.host_;
        optimized_ = other.optimized_;
        sz = other.sz;
    }
    return *this;
}

Valuable ConcreteValuable::operator-() const noexcept {
    return ConcreteValuable("-" + expression_, host_, optimized_);
}

Valuable& ConcreteValuable::operator+=(const Valuable& v) noexcept {
    expression_ += "+" + dynamic_cast<const ConcreteValuable&>(v).expression_;
    return *this;
}

Valuable& ConcreteValuable::operator*=(const Valuable& v) noexcept {
    expression_ = "(" + expression_ + ")*(" + dynamic_cast<const ConcreteValuable&>(v).expression_ + ")";
    return *this;
}

bool ConcreteValuable::MultiplyIfSimplifiable(const Valuable& v) noexcept {
    return false;  // Default implementation
}

bool ConcreteValuable::SumIfSimplifiable(const Valuable& v) noexcept {
    return false;  // Default implementation
}

Valuable& ConcreteValuable::operator/=(const Valuable& v) noexcept {
    expression_ = "(" + expression_ + ")/(" + dynamic_cast<const ConcreteValuable&>(v).expression_ + ")";
    return *this;
}

Valuable& ConcreteValuable::operator%=(const Valuable& v) noexcept {
    expression_ = "(" + expression_ + ")%(" + dynamic_cast<const ConcreteValuable&>(v).expression_ + ")";
    return *this;
}

Valuable& ConcreteValuable::operator^=(const Valuable& v) noexcept {
    expression_ = "(" + expression_ + ")^(" + dynamic_cast<const ConcreteValuable&>(v).expression_ + ")";
    return *this;
}

Valuable& ConcreteValuable::d(const Variable& x) noexcept {
    return *this;  // Default implementation
}

bool ConcreteValuable::operator<(const Valuable& v) const noexcept {
    return expression_ < dynamic_cast<const ConcreteValuable&>(v).expression_;
}

bool ConcreteValuable::operator==(const Valuable& v) const noexcept {
    return expression_ == dynamic_cast<const ConcreteValuable&>(v).expression_;
}

bool ConcreteValuable::operator==(const Variable& v) const noexcept {
    return false;  // Default implementation
}

Valuable ConcreteValuable::operator()(const Variable& va, const Valuable& augmentation) const {
    return *this;  // Default implementation
}

} // namespace omnn::math
