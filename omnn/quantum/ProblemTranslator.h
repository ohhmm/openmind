#pragma once

#include <string>
#include <omnn/math/Valuable.h>

namespace omnn {
namespace quantum {

class ProblemTranslator {
public:
    // Translates a Valuable polynomial into Dirac JSON
    static std::string Translate(const omnn::math::Valuable& objective);
};

} // namespace quantum
} // namespace omnn
