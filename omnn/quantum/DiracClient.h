#pragma once

#include <string>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/asio/ssl.hpp>

namespace omnn {
namespace quantum {

class DiracClient {
public:
    // Optional constructor parameters fall back to QCI_TOKEN/QCI_LOGIN/QCI_PASSWORD
    // and QCI_API_URL environment variables.
    DiracClient(const std::string& token = "", const std::string& apiUrl = "");
    ~DiracClient() = default;

    // API endpoints
    std::string UploadProblem(const std::string& jsonProblem);
    std::string CreateJob(const std::string& problemId, const std::string& device = "dirac-3");
    std::string PollJobStatus(const std::string& jobId);
    std::string GetJobResult(const std::string& jobId);

private:
    std::string _refreshToken;
    std::string _apiUrl;
    std::string _accessToken;

    std::string getAccessToken();
    static std::string extractJsonValue(const std::string& json, const std::string& key);

    // Internal helper for HTTPS requests
    std::string performRequest(const std::string& endpoint,
                               const std::string& method,
                               const std::string& payload = "",
                               bool authenticated = true);
};

} // namespace quantum
} // namespace omnn
