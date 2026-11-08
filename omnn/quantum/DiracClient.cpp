#include "DiracClient.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/json.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <cstdlib>
#include <stdexcept>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

namespace omnn {
namespace quantum {

static std::string envOr(const char* name, const std::string& fallback = "") {
    const char* value = std::getenv(name);
    if (!value) return fallback;
    return value;
}

DiracClient::DiracClient(const std::string& token, const std::string& apiUrl) {
    if (!token.empty()) {
        _refreshToken = token;
    } else if (!envOr("QCI_TOKEN").empty()) {
        _refreshToken = envOr("QCI_TOKEN");
    } else if (!envOr("QCI_PASSWORD").empty()) {
        _refreshToken = envOr("QCI_PASSWORD");
    } else if (!envOr("QCI_LOGIN").empty()) {
        _refreshToken = envOr("QCI_LOGIN");
    } else {
        throw std::runtime_error("QCI_TOKEN (or QCI_LOGIN/QCI_PASSWORD) environment variable must be set");
    }

    if (!apiUrl.empty()) {
        _apiUrl = apiUrl;
    } else if (!envOr("QCI_API_URL").empty()) {
        _apiUrl = envOr("QCI_API_URL");
    } else {
        _apiUrl = "https://api.qci-prod.com";
    }
}

std::string DiracClient::extractJsonValue(const std::string& json, const std::string& key) {
    std::string needle = "\"" + key + "\"";
    size_t pos = json.find(needle);
    if (pos == std::string::npos) return "";
    pos = json.find(':', pos);
    if (pos == std::string::npos) return "";
    size_t valueStart = json.find_first_not_of(" \t\r\n", pos + 1);
    if (valueStart == std::string::npos) return "";
    if (json[valueStart] == '"') {
        size_t end = json.find('"', valueStart + 1);
        if (end == std::string::npos) return "";
        return json.substr(valueStart + 1, end - valueStart - 1);
    }
    size_t end = json.find_first_of(",}]", valueStart);
    if (end == std::string::npos) end = json.size();
    return json.substr(valueStart, end - valueStart);
}

std::string DiracClient::getAccessToken() {
    if (!_accessToken.empty()) return _accessToken;

    try {
        std::string body = performRequest("/auth/v1/access-tokens/", "POST",
                                         "{\"refresh_token\": \"" + _refreshToken + "\"}", false);
        _accessToken = extractJsonValue(body, "access_token");
    } catch (...) {
        _accessToken.clear();
    }
    if (_accessToken.empty()) {
        // Accept a caller-provided access token directly.
        _accessToken = _refreshToken;
    }
    return _accessToken;
}

std::string DiracClient::performRequest(const std::string& endpoint,
                                       const std::string& method,
                                       const std::string& payload,
                                       bool authenticated) {
    std::string host;
    std::string path = endpoint;

    std::string url = _apiUrl;
    size_t protocol_pos = url.find("://");
    if (protocol_pos != std::string::npos) {
        url = url.substr(protocol_pos + 3);
    }

    size_t host_end = url.find("/");
    std::string basePath;
    if (host_end == std::string::npos) {
        host = url;
    } else {
        host = url.substr(0, host_end);
        basePath = url.substr(host_end);
    }
    if (basePath.size() > 1 && basePath.back() == '/') basePath.pop_back();
    if (!basePath.empty()) path = basePath + (endpoint.empty() || endpoint.front() == '/' ? endpoint : "/" + endpoint);

    net::io_context ioc;
    ssl::context ctx(ssl::context::tlsv12_client);
    ctx.set_default_verify_paths();

    tcp::resolver resolver(ioc);
    beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

    if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
        throw std::runtime_error("Failed to set SNI hostname");
    }

    auto const results = resolver.resolve(host, "443");
    beast::get_lowest_layer(stream).connect(results);
    stream.handshake(ssl::stream_base::client);

    http::request<http::string_body> req;
    req.version(11);
    if (method == "POST") req.method(http::verb::post);
    else if (method == "PUT") req.method(http::verb::put);
    else if (method == "PATCH") req.method(http::verb::patch);
    else if (method == "DELETE") req.method(http::verb::delete_);
    else req.method(http::verb::get);

    req.target(path);
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    if (authenticated) {
        req.set(http::field::authorization, "Bearer " + getAccessToken());
    }
    req.set(http::field::content_type, "application/json");

    if (!payload.empty()) {
        req.body() = payload;
        req.prepare_payload();
    }

    http::write(stream, req);

    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);

    beast::error_code ec;
    stream.shutdown(ec);

    if (res.result() < http::status::ok || res.result() >= http::status::multiple_choices) {
        throw std::runtime_error("API request failed with status " + std::to_string(static_cast<int>(res.result())) + ": " + res.body());
    }

    return res.body();
}

std::string DiracClient::UploadProblem(const std::string& jsonProblem) {
    namespace json = boost::json;
    json::value parsed = json::parse(jsonProblem);
    const json::object& root = parsed.as_object();

    std::string fileName = "file.json";
    if (root.contains("file_name") && root.at("file_name").is_string()) {
        fileName = std::string(root.at("file_name").as_string());
    }

    const json::object& fileConfig = root.at("file_config").as_object();
    if (fileConfig.empty()) throw std::runtime_error("file_config must contain exactly one file type");
    auto it = fileConfig.begin();
    std::string fileType = std::string(it->key());
    const json::object& typeConfig = it->value().as_object();

    int64_t numVars = 0;
    if (typeConfig.contains("num_variables") && typeConfig.at("num_variables").is_int64()) {
        numVars = typeConfig.at("num_variables").as_int64();
    } else if (typeConfig.contains("num_variables") && typeConfig.at("num_variables").is_uint64()) {
        numVars = static_cast<int64_t>(typeConfig.at("num_variables").as_uint64());
    } else if (typeConfig.contains("num_variables") && typeConfig.at("num_variables").is_double()) {
        numVars = static_cast<int64_t>(typeConfig.at("num_variables").as_double());
    } else {
        throw std::runtime_error("file_config.<type>.num_variables is required");
    }

    json::object metaTypeConfig;
    metaTypeConfig["num_variables"] = numVars;
    json::object metaFileConfig;
    metaFileConfig[fileType] = metaTypeConfig;
    json::object metadata;
    metadata["file_name"] = fileName;
    metadata["file_config"] = metaFileConfig;

    std::string uploadResponse = performRequest("/optimization/v1/files", "POST", json::serialize(metadata));
    std::string fileId = extractJsonValue(uploadResponse, "file_id");
    if (fileId.empty()) throw std::runtime_error("file upload response missing file_id: " + uploadResponse);

    json::object patchTypeConfig;
    if (typeConfig.contains("data")) patchTypeConfig["data"] = typeConfig.at("data");
    json::object patchFileConfig;
    patchFileConfig[fileType] = patchTypeConfig;
    json::object patch;
    patch["file_config"] = patchFileConfig;

    performRequest("/optimization/v1/files/" + fileId + "/contents/1", "PATCH", json::serialize(patch));
return"{\"file_id\":\""+fileId+"\"}";
}

std::string DiracClient::CreateJob(const std::string& problemId, const std::string& device) {
    std::string payload = "{\"job_submission\":{\"problem_config\":{\"normalized_qudit_hamiltonian_optimization_continuous\":{\"hamiltonian_file_id\":\"" + problemId + "\"}},\"device_config\":{\"" + device + "\":{}}}}";
return performRequest("/optimization/v1/jobs", "POST", payload);
}

std::string DiracClient::PollJobStatus(const std::string& jobId) {
    std::string body = performRequest("/optimization/v1/jobs/" + jobId + "/status", "GET");
    std::string status = extractJsonValue(body, "status");
    if (!status.empty()) return status;
    return body;
}

std::string DiracClient::GetJobResult(const std::string& jobId) {
    std::string info = performRequest("/optimization/v1/jobs/" + jobId, "GET");
    std::string fileId = extractJsonValue(info, "file_id");
    if (!fileId.empty()) {
        return performRequest("/optimization/v1/files/" + fileId + "/contents/1", "GET");
    }
    return info;
}

} // namespace quantum
} // namespace omnn
