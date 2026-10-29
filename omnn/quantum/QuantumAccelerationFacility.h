#pragma once

#include <OpenMind/Facility.h>
#include <omnn/math/Valuable.h>
#include <omnn/quantum/DiracClient.h>
#include <map>
#include <memory>

namespace omnn {
namespace quantum {

class QuantumAccelerationFacility : public Facility {
public:
    QuantumAccelerationFacility();
    ~QuantumAccelerationFacility() override = default;

    void SetObjective(std::shared_ptr<omnn::math::Valuable> objective);

    bool Invoke() override;

    std::map<omnn::math::Variable, int> GetOptimalSolution() const;

private:
    std::shared_ptr<omnn::math::Valuable> _objective;
    DiracClient _client;
    std::map<omnn::math::Variable, int> _bestAssignment;
};

} // namespace quantum
} // namespace omnn
