//
// Created by Sergej Krivonos on 07.11.26.
//

#include "ConjectureSolver.h"

#include <boost/multiprecision/cpp_int.hpp>

#include <algorithm>
#include <cctype>
#include <functional>
#include <limits>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace omnn::math {
namespace {

using boost::multiprecision::cpp_int;

struct Node {
    std::map<std::string, std::int64_t> assigns;
    std::vector<Node> items;
};

class SpecParser {
public:
    explicit SpecParser(std::string_view in)
        : s(in) {}

    Node Parse() {
        auto node = ParseObject();
        Skip();
        if (i != s.size())
            throw std::runtime_error("trailing data in conjecture spec");
        return node;
    }

private:
    std::string_view s;
    std::size_t i = 0;

    void Skip() {
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i])))
            ++i;
        if (i < s.size() && s[i] == ',') {
            ++i;
            Skip();
        }
    }

    bool Eat(char c) {
        Skip();
        if (i < s.size() && s[i] == c) {
            ++i;
            return true;
        }
        return false;
    }

    [[noreturn]] void Fail(const char* what) const {
        throw std::runtime_error(what);
    }

    std::string ParseName() {
        Skip();
        if (i >= s.size() || !std::isalpha(static_cast<unsigned char>(s[i])))
            Fail("expected name");
        auto begin = i;
        ++i;
        while (i < s.size() && std::isalnum(static_cast<unsigned char>(s[i])))
            ++i;
        return std::string(s.substr(begin, i - begin));
    }

    std::int64_t ParseNumber() {
        Skip();
        int sign = 1;
        if (i < s.size() && (s[i] == '-' || s[i] == '+')) {
            if (s[i] == '-')
                sign = -1;
            ++i;
        }
        if (i >= s.size() || !std::isdigit(static_cast<unsigned char>(s[i])))
            Fail("expected number");
        std::int64_t value = 0;
        while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) {
            value = value * 10 + (s[i] - '0');
            ++i;
        }
        return value * sign;
    }

    Node ParseObject() {
        if (!Eat('{'))
            Fail("expected '{'");
        Node node;
        Skip();
        while (!Eat('}')) {
            Skip();
            if (i >= s.size())
                Fail("unclosed '{'");
            if (s[i] == '[') {
                auto items = ParseArray();
                node.items.insert(node.items.end(), items.begin(), items.end());
            } else if (s[i] == '{') {
                node.items.push_back(ParseObject());
            } else {
                auto name = ParseName();
                if (!Eat('='))
                    Fail("expected '='");
                node.assigns.emplace(std::move(name), ParseNumber());
            }
            Skip();
        }
        return node;
    }

    std::vector<Node> ParseArray() {
        if (!Eat('['))
            Fail("expected '['");
        std::vector<Node> items;
        Skip();
        while (!Eat(']')) {
            if (i >= s.size())
                Fail("unclosed '['");
            items.push_back(ParseObject());
            Skip();
        }
        return items;
    }
};

void Flatten(const Node& node, ConjectureSolver::row_t inherited, std::vector<ConjectureSolver::row_t>& rows) {
    for (auto& [name, value] : node.assigns)
        inherited[name] = value;
    if (node.items.empty())
        rows.push_back(std::move(inherited));
    else
        for (auto& item : node.items)
            Flatten(item, inherited, rows);
}

std::int64_t Gcd(std::int64_t a, std::int64_t b) {
    a = std::llabs(a);
    b = std::llabs(b);
    while (b) {
        auto t = a % b;
        a = b;
        b = t;
    }
    return a == 0 ? 1 : a;
}

struct Mono {
    std::vector<std::pair<std::string, int>> factors;
    int degree = 0;
    std::string name;
    std::vector<std::int64_t> at;
};

struct Term {
    int coeff = 0;
    int mono = 0;
};

enum class Shape { Identity, Times, Over };

struct Scored {
    int terms = 0;
    int magnitude = 0;
    std::string expression;

    bool BetterThan(const Scored& o) const {
        if (terms != o.terms)
            return terms < o.terms;
        if (magnitude != o.magnitude)
            return magnitude < o.magnitude;
        return expression < o.expression;
    }
};

std::vector<Mono> Monomials(const std::vector<std::string>& names, const std::vector<ConjectureSolver::row_t>& rows) {
    struct Builder {
        std::vector<std::pair<std::string, int>> factors;
        int degree = 0;
    };
    std::vector<Builder> builders{{}};
    for (auto& name : names) {
        std::vector<Builder> extra;
        for (auto& built : builders) {
            if (built.degree >= 2)
                continue;
            Builder once = built;
            once.factors.push_back({name, 1});
            once.degree += 1;
            extra.push_back(once);
            if (built.degree == 0) {
                Builder twice = built;
                twice.factors.push_back({name, 2});
                twice.degree = 2;
                extra.push_back(twice);
            }
        }
        builders.insert(builders.end(), extra.begin(), extra.end());
    }

    std::vector<Mono> monos;
    monos.reserve(builders.size());
    for (auto& built : builders) {
        Mono mono;
        mono.factors = std::move(built.factors);
        mono.degree = built.degree;
        if (mono.factors.empty()) {
            mono.name = "1";
        } else {
            for (std::size_t f = 0; f < mono.factors.size(); ++f) {
                if (f)
                    mono.name += '*';
                mono.name += mono.factors[f].first;
                if (mono.factors[f].second != 1)
                    mono.name += '^' + std::to_string(mono.factors[f].second);
            }
        }
        mono.at.reserve(rows.size());
        for (auto& row : rows) {
            std::int64_t value = 1;
            for (auto& [name, power] : mono.factors) {
                auto found = row.find(name);
                if (found == row.end())
                    throw std::runtime_error("missing sample value");
                for (int p = 0; p < power; ++p)
                    value *= found->second;
            }
            mono.at.push_back(value);
        }
        monos.push_back(std::move(mono));
    }
    return monos;
}

std::int64_t EvalTerms(const std::vector<Term>& terms, const std::vector<Mono>& monos, std::size_t row) {
    std::int64_t sum = 0;
    for (auto& term : terms)
        sum += static_cast<std::int64_t>(term.coeff) * monos[term.mono].at[row];
    return sum;
}

bool DenominatorIsOne(const std::vector<Term>& den, const std::vector<Mono>& monos) {
    return den.size() == 1 && den[0].coeff == 1 && monos[den[0].mono].degree == 0;
}

int Magnitude(const std::vector<Term>& num, const std::vector<Term>& den) {
    int mag = 0;
    for (auto& term : num)
        mag += std::abs(term.coeff);
    for (auto& term : den)
        mag += std::abs(term.coeff);
    return mag;
}

std::string RenderSum(std::vector<Term> terms, const std::vector<Mono>& monos) {
    std::sort(terms.begin(), terms.end(), [&](const Term& a, const Term& b) {
        if (monos[a.mono].degree != monos[b.mono].degree)
            return monos[a.mono].degree > monos[b.mono].degree;
        return monos[a.mono].name < monos[b.mono].name;
    });
    std::string out;
    for (std::size_t t = 0; t < terms.size(); ++t) {
        auto coeff = terms[t].coeff;
        auto& mono = monos[terms[t].mono];
        if (t == 0) {
            if (coeff < 0)
                out += '-';
        } else {
            out += coeff < 0 ? '-' : '+';
        }
        auto absCoeff = std::abs(coeff);
        auto showNumber = absCoeff != 1 || mono.degree == 0;
        if (showNumber)
            out += std::to_string(absCoeff);
        if (mono.degree != 0) {
            if (showNumber)
                out += '*';
            out += mono.name;
        }
    }
    return out;
}

void Canonicalize(std::vector<Term>& num, std::vector<Term>& den, const std::vector<Mono>& monos) {
    auto rendered = den;
    std::sort(rendered.begin(), rendered.end(), [&](const Term& a, const Term& b) {
        if (monos[a.mono].degree != monos[b.mono].degree)
            return monos[a.mono].degree > monos[b.mono].degree;
        return monos[a.mono].name < monos[b.mono].name;
    });
    if (rendered[0].coeff < 0) {
        for (auto& term : num)
            term.coeff = -term.coeff;
        for (auto& term : den)
            term.coeff = -term.coeff;
    }
}

std::string Render(const std::vector<Term>& num, const std::vector<Term>& den, const std::vector<Mono>& monos,
    Shape shape, const std::string& var) {
    auto numerator = RenderSum(num, monos);
    auto denominator = RenderSum(den, monos);
    std::string body;
    if (denominator == "1")
        body = numerator.find_first_of("+-", 1) == std::string::npos ? numerator : '(' + numerator + ')';
    else
        body = '(' + numerator + ")/(" + denominator + ')';

    if (shape == Shape::Identity)
        return body;
    if (shape == Shape::Times) {
        if (body == "1")
            return var;
        if (body.find_first_of("()+") == std::string::npos && body.front() != '-')
            return body + '*' + var;
        return var + '*' + body;
    }
    if (body == "1")
        return "1/" + var;
    return body + '/' + var;
}

bool Matches(const std::vector<Term>& num, const std::vector<Term>& den, const std::vector<Mono>& monos,
    Shape shape, const std::string& var, const std::string& target, const std::vector<ConjectureSolver::row_t>& rows) {
    for (std::size_t r = 0; r < rows.size(); ++r) {
        auto n = EvalTerms(num, monos, r);
        auto d = EvalTerms(den, monos, r);
        if (d == 0)
            return false;
        auto x = rows[r].at(target);
        cpp_int left = n;
        cpp_int right = cpp_int(x) * d;
        if (shape == Shape::Times) {
            left *= rows[r].at(var);
        } else if (shape == Shape::Over) {
            right *= rows[r].at(var);
        }
        if (left != right)
            return false;
    }
    return true;
}

void Consider(std::vector<Term> num, std::vector<Term> den, const std::vector<Mono>& monos, Shape shape,
    const std::string& var, const std::string& target, const std::vector<ConjectureSolver::row_t>& rows,
    std::optional<Scored>& best) {
    if (!Matches(num, den, monos, shape, var, target, rows))
        return;
    Canonicalize(num, den, monos);
    Scored scored;
    scored.terms = static_cast<int>(num.size() + (DenominatorIsOne(den, monos) ? 0 : den.size()));
    scored.magnitude = Magnitude(num, den);
    scored.expression = Render(num, den, monos, shape, var);
    if (!best || scored.BetterThan(*best))
        best = std::move(scored);
}

void Choose(int from, int need, const std::vector<Mono>& monos, std::vector<Term>& chosen, const auto& emit) {
    if (need == 0) {
        emit();
        return;
    }
    constexpr int coeffs[] = {1, -1, 2, -2};
    for (int mono = from; mono < static_cast<int>(monos.size()); ++mono) {
        for (int coeff : coeffs) {
            chosen.push_back(Term{coeff, mono});
            Choose(mono + 1, need - 1, monos, chosen, emit);
            chosen.pop_back();
        }
    }
}

std::string FractionTimes(std::int64_t num, std::int64_t den, const std::string& var, bool over) {
    auto g = Gcd(num, den);
    num /= g;
    den /= g;
    if (den < 0) {
        den = -den;
        num = -num;
    }
    std::ostringstream out;
    if (den == 1) {
        if (num == 1 && !over)
            return var;
        if (num == -1 && !over)
            return "-" + var;
        if (num == 1 && over)
            return "1/" + var;
        out << num << (over ? "/" : "*") << var;
        return out.str();
    }
    if (over)
        out << '(' << num << '/' << den << ")/" << var;
    else
        out << '(' << num << '/' << den << ")*" << var;
    return out.str();
}

std::optional<std::string> RationalFallback(const std::string& target, const std::vector<std::string>& inputs,
    const std::vector<ConjectureSolver::row_t>& rows) {
    for (auto& var : inputs) {
        bool proportional = true;
        std::int64_t n0 = 0;
        std::int64_t d0 = 0;
        bool inverse = true;
        bool haveProduct = false;
        cpp_int product = 0;
        for (auto& row : rows) {
            auto x = row.at(target);
            auto v = row.at(var);
            if (v == 0 || (d0 != 0 && cpp_int(x) * d0 != cpp_int(n0) * v))
                proportional = false;
            else if (d0 == 0) {
                n0 = x;
                d0 = v;
            }
            if (v == 0 || (haveProduct && cpp_int(x) * v != product))
                inverse = false;
            else if (!haveProduct) {
                product = cpp_int(x) * v;
                haveProduct = true;
            }
        }
        if (proportional && d0 != 0)
            return FractionTimes(n0, d0, var, false);
        if (inverse && haveProduct && product <= std::numeric_limits<std::int64_t>::max()
            && product >= std::numeric_limits<std::int64_t>::min())
            return FractionTimes(static_cast<std::int64_t>(product), 1, var, true);
    }
    return std::nullopt;
}

} // namespace

ConjectureSolver ConjectureSolver::Parse(std::string_view spec) {
    auto root = SpecParser(spec).Parse();
    ConjectureSolver solver;
    Flatten(root, {}, solver.rows);
    if (solver.rows.empty())
        throw std::runtime_error("conjecture spec has no samples");
    return solver;
}

void ConjectureSolver::Add(row_t row) { rows.push_back(std::move(row)); }

std::string ConjectureSolver::Solve(std::string_view targetView) const {
    if (rows.empty())
        return {};
    const std::string target(targetView);
    for (auto& row : rows) {
        if (!row.count(target))
            return {};
    }

    auto constant = rows.front().at(target);
    if (std::all_of(rows.begin(), rows.end(), [&](auto& row) { return row.at(target) == constant; }))
        return std::to_string(constant);

    std::vector<std::string> inputs;
    for (auto& [name, _] : rows.front()) {
        if (name != target)
            inputs.push_back(name);
    }
    std::sort(inputs.begin(), inputs.end());

    for (auto& name : inputs) {
        bool same = true;
        for (auto& row : rows) {
            auto found = row.find(name);
            if (found == row.end() || found->second != row.at(target)) {
                same = false;
                break;
            }
        }
        if (same)
            return name;
    }

    std::optional<Scored> best;
    for (int complexity = 1; complexity <= 5 && !best; ++complexity) {
        for (auto& var : inputs) {
            std::vector<std::string> params;
            for (auto& name : inputs)
                if (name != var)
                    params.push_back(name);
            auto monos = Monomials(params, rows);
            if (monos.empty())
                continue;
            const std::size_t rowCount = monos.front().at.size();
            for (int numN = 1; numN <= 3 && !best; ++numN) {
                for (int denN = 1; denN <= 2 && !best; ++denN) {
                    // Collect all numerator term combinations keyed by their
                    // row-evaluation vector, so denominators can be matched
                    // directly instead of testing every pair.
                    std::map<std::vector<cpp_int>, std::vector<std::vector<Term>>> numByEval;
                    {
                        std::function<void(int, int, std::vector<Term>&, std::vector<cpp_int>)> genNum =
                            [&](int from, int need, std::vector<Term>& chosen, std::vector<cpp_int> eval) {
                                if (need == 0) {
                                    numByEval[eval].push_back(chosen);
                                    return;
                                }
                                constexpr int coeffs[] = {1, -1, 2, -2};
                                for (int mono = from; mono < static_cast<int>(monos.size()); ++mono) {
                                    for (int coeff : coeffs) {
                                        chosen.push_back(Term{coeff, mono});
                                        std::vector<cpp_int> next = eval;
                                        for (std::size_t r = 0; r < rowCount; ++r)
                                            next[r] += coeff * monos[mono].at[r];
                                        genNum(mono + 1, need - 1, chosen, next);
                                        chosen.pop_back();
                                    }
                                }
                            };
                        std::vector<Term> chosen;
                        std::vector<cpp_int> zeros(rowCount, 0);
                        genNum(0, numN, chosen, zeros);
                    }

                    std::function<void(int, int, std::vector<Term>&, std::vector<cpp_int>)> genDen =
                        [&](int from, int need, std::vector<Term>& chosen, std::vector<cpp_int> eval) {
                            if (best)
                                return;
                            if (need == 0) {
                                auto cost = numN + (DenominatorIsOne(chosen, monos) ? 0 : denN);
                                if (cost != complexity)
                                    return;
                                for (auto shape : {Shape::Times, Shape::Over, Shape::Identity}) {
                                    std::vector<cpp_int> targetEval(rowCount, 0);
                                    bool feasible = true;
                                    for (std::size_t r = 0; r < rowCount; ++r) {
                                        auto x = rows[r].at(target);
                                        auto v = rows[r].at(var);
                                        switch (shape) {
                                        case Shape::Identity:
                                            targetEval[r] = cpp_int(x) * eval[r];
                                            break;
                                        case Shape::Times:
                                            if (v == 0) {
                                                feasible = false;
                                            } else {
                                                cpp_int product = cpp_int(x) * eval[r];
                                                if (product % v != 0) {
                                                    feasible = false;
                                                } else {
                                                    targetEval[r] = product / v;
                                                }
                                            }
                                            break;
                                        case Shape::Over:
                                            targetEval[r] = cpp_int(x) * eval[r] * v;
                                            break;
                                        }
                                        if (!feasible)
                                            break;
                                    }
                                    if (!feasible)
                                        continue;
                                    auto found = numByEval.find(targetEval);
                                    if (found == numByEval.end())
                                        continue;
                                    for (auto numTerms : found->second) {
                                        std::vector<Term> num = numTerms;
                                        std::vector<Term> den = chosen;
                                        Consider(num, den, monos, shape, var, target, rows, best);
                                        if (best)
                                            return;
                                    }
                                }
                                return;
                            }
                            constexpr int coeffs[] = {1, -1, 2, -2};
                            for (int mono = from; mono < static_cast<int>(monos.size()); ++mono) {
                                for (int coeff : coeffs) {
                                    chosen.push_back(Term{coeff, mono});
                                    std::vector<cpp_int> next = eval;
                                    for (std::size_t r = 0; r < rowCount; ++r)
                                        next[r] += coeff * monos[mono].at[r];
                                    genDen(mono + 1, need - 1, chosen, next);
                                    chosen.pop_back();
                                    if (best)
                                        return;
                                }
                            }
                        };

                        std::vector<Term> denEmpty;
                        std::vector<cpp_int> zeros(rowCount, 0);
                        genDen(0, denN, denEmpty, zeros);
                }
            }
        }
    }
    if (best)
        return best->expression;
    if (auto fallback = RationalFallback(target, inputs, rows))
        return *fallback;
    return {};
}

} // namespace omnn::math
