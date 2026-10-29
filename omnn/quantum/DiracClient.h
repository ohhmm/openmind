#pragma once

#include <string>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/asio/ssl.hpp>

namespace omnn {
namespace quantum {

class DiracClient {
public:
    DiracClient();
    ~DiracClient() = default;

    // API endpoints
    std::string UploadProblem(const std::string& jsonProblem);
    std::string CreateJob(const std::string& problemId, const std::string& device = "dirac-3");
    std::string PollJobStatus(const std::string& jobId);
    std::string GetJobResult(const std::string& jobId);

private:
    std::string _token;
    std::string _apiUrl;

    // Internal helper for HTTPS requests
    std::string performRequest(const std::string& endpoint,
                               const std::string& method,
                               const std::string& payload = "");
};

} // namespace quantum
} // namespace omnn
