#include "QuantumAccelerationFacility.h"
#include "DiracClient.h"
#include "ProblemTranslator.h"
#include <omnn/math/Variable.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <sstream>

namespace omnn {
namespace quantum {

QuantumAccelerationFacility::QuantumAccelerationFacility() : _objective(nullptr) {}

void QuantumAccelerationFacility::SetObjective(std::shared_ptr<omnn::math::Valuable> objective) {
    _objective = objective;
}

bool QuantumAccelerationFacility::Invoke() {
    if (!_objective) {
        std::cerr << "QuantumAccelerationFacility: No objective set" << std::endl;
        return false;
    }

    try {
        // 1. Translation
        std::string jsonProblem = ProblemTranslator::Translate(*_objective);

        // 2. Upload
        std::string problemId = _client.UploadProblem(jsonProblem);

        // 3. Create Job
        std::string jobId = _client.CreateJob(problemId, "dirac-3");

        // 4. Polling loop
        std::string status = "pending";
        while (status != "completed" && status != "failed") {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            status = _client.PollJobStatus(jobId);
        }

        if (status == "failed") {
            return false;
        }

        // 5. Result retrieval
        std::string resultJson = _client.GetJobResult(jobId);

        // Simple JSON parsing for the optimal assignment: {"x0": 1, "x1": 0, ...}
        // In a real implementation, use a JSON library like nlohmann/json
        _bestAssignment.clear();
        size_t pos = 0;
        while ((pos = resultJson.find("\"", pos)) != std::string::npos) {
            size_t end_var = resultJson.find("\"", pos + 1);
            std::string varName = resultJson.substr(pos + 1, end_var - pos - 1);

            size_t colon = resultJson.find(":", end_var);
            size_t comma = resultJson.find_first_of(",}", colon);
            std::string valStr = resultJson.substr(colon + 1, comma - colon - 1);

            // Convert varName to Variable and valStr to int
            // For this implementation, we'll assume variables are managed by a VarHost
            // In practice, we'd use a mapping from the objective function
            try {
                _bestAssignment[omnn::math::Variable(varName.c_str())] = std::stoi(valStr);
            } catch (...) {}

            pos = comma + 1;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "QuantumAccelerationFacility error: " << e.what() << std::endl;
        return false;
    }
}

std::map<omnn::math::Variable, int> QuantumAccelerationFacility::GetOptimalSolution() const {
    return _bestAssignment;
}

} // namespace quantum
} // namespace omnn
