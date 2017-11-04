#include "Accessor.h"

namespace omnn{
namespace extrapolator {

Accessor::Accessor(Valuable& m, size_t& hash)
: v(m), h(hash)
{
}

Valuable Accessor::operator -() const
{
    return -v;
}

Valuable& Accessor::operator +=(const Valuable& v)
{
    h ^= v.Hash();
    this->v += v;
    h ^= v.Hash();
    return *this;
}

Valuable& Accessor::operator *=(const Valuable& v)
{
    h ^= v.Hash();
    this->v *= v;
    h ^= v.Hash();
    return *this;
}

Valuable& Accessor::operator /=(const Valuable& v)
{
    h ^= v.Hash();
    this->v /= v;
    h ^= v.Hash();
    return *this;
}

Valuable& Accessor::operator %=(const Valuable& v)
{
    h ^= v.Hash();
    this->v %= v;
    h ^= v.Hash();
    return *this;
}

Valuable& Accessor::operator --()
{
    h ^= v.Hash();
    --this->v;
    h ^= v.Hash();
    return *this;
}

Valuable& Accessor::operator ++()
{
    h ^= v.Hash();
    ++this->v;
    h ^= v.Hash();
    return *this;
}

Valuable& Accessor::operator ^=(const Valuable& v)
{
    h ^= v.Hash();
    this->v ^= v;
    h ^= v.Hash();
    return *this;
}

bool Accessor::operator <(const Valuable& v) const
{
    return this->v < v;
}

bool Accessor::operator ==(const Valuable& v) const
{
    return this->v == v;
}

const Variable* Accessor::FindVa() const
{
    return v.FindVa();
}

void Accessor::CollectVa(std::set<Variable>& v) const
{
    this->v.CollectVa(v);
}

void Accessor::Eval(const Variable& va, const Valuable& v)
{
    h ^= v.Hash();
    this->v.Eval(va, v);
    h ^= v.Hash();
}

}}
