#include "DiracClient.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <cstdlib>
#include <cctype>
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
        // Keep construction possible without credentials; Invoke() reports failure.
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
    std::string authority;
    std::string basePath;
    if (host_end == std::string::npos) {
        authority = url;
    } else {
        authority = url.substr(0, host_end);
        basePath = url.substr(host_end);
    }
    if (basePath.size() > 1 && basePath.back() == '/') basePath.pop_back();
    if (!basePath.empty()) path = basePath + (endpoint.empty() || endpoint.front() == '/' ? endpoint : "/" + endpoint);

    std::string port = "443";
    host = authority;
    size_t colon = authority.rfind(':');
    if (colon != std::string::npos && (authority.find(']') == std::string::npos || authority.find(']') < colon)) {
        host = authority.substr(0, colon);
        port = authority.substr(colon + 1);
    }

    bool secure = _apiUrl.rfind("https://", 0) == 0;

    if (!secure) {
        net::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);
        auto results = resolver.resolve(host, port);
        stream.connect(results);

        http::request<http::string_body> req;
        req.version(11);
        if (method == "POST") req.method(http::verb::post);
        else if (method == "PUT") req.method(http::verb::put);
        else if (method == "DELETE") req.method(http::verb::delete_);
        else req.method(http::verb::get);
        req.target(path);
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        if (authenticated) {
            req.set(http::field::authorization, "Bearer " + getAccessToken());
        }
        req.set(http::field::content_type, "application/json");
        req.set(http::field::connection, "close");
        if (!payload.empty()) {
            req.body() = payload;
            req.prepare_payload();
        }
        http::write(stream, req);

        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        if (res.result() != http::status::ok) {
            throw std::runtime_error("API request failed with status " + std::to_string(static_cast<int>(res.result())) + ": " + res.body());
        }
        return res.body();
    }

    net::io_context ioc;
    ssl::context ctx(ssl::context::tlsv12_client);
    ctx.set_default_verify_paths();

    tcp::resolver resolver(ioc);
    beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

    if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
        throw std::runtime_error("Failed to set SNI hostname");
    }

    auto const results = resolver.resolve(host, port);
    beast::get_lowest_layer(stream).connect(results);
    stream.handshake(ssl::stream_base::client);

    http::request<http::string_body> req;
    req.version(11);
    if (method == "POST") req.method(http::verb::post);
    else if (method == "PUT") req.method(http::verb::put);
    else if (method == "DELETE") req.method(http::verb::delete_);
    else req.method(http::verb::get);

    req.target(path);
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    if (authenticated) {
        req.set(http::field::authorization, "Bearer " + getAccessToken());
    }
    req.set(http::field::content_type, "application/json");
    req.set(http::field::connection, "close");

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

    if (res.result() != http::status::ok) {
        throw std::runtime_error("API request failed with status " + std::to_string(static_cast<int>(res.result())) + ": " + res.body());
    }

    return res.body();
}

std::string DiracClient::extractId(const std::string& body) {
    for (const char* key : {"file_id", "job_id", "jobId", "id", "problem_id"}) {
        std::string value = extractJsonValue(body, key);
        if (!value.empty())
            return value;
    }
    std::string trimmed = body;
    trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);
    size_t start = trimmed.find_first_not_of(" \t\r\n");
    if (start != std::string::npos)
        trimmed.erase(0, start);
    if (trimmed.size() >= 2 && trimmed.front() == '"' && trimmed.back() == '"')
        trimmed = trimmed.substr(1, trimmed.size() - 2);
    return trimmed;
}

std::string DiracClient::UploadProblem(const std::string& jsonProblem) {
    return extractId(performRequest("/optimization/v1/files", "POST", jsonProblem));
}

std::string DiracClient::CreateJob(const std::string& problemId, const std::string& device) {
    std::string payload = "{\"job_submission\":{\"problem_config\":{},\"device_config\":{\""
                          + device + "\":{}}},\"file_id\":\"" + problemId + "\"}";
    return extractId(performRequest("/optimization/v1/jobs", "POST", payload));
}

std::string DiracClient::PollJobStatus(const std::string& jobId) {
    std::string body = performRequest("/optimization/v1/jobs/" + jobId + "/status", "GET");
    std::string status = extractJsonValue(body, "status");
    if (status.empty())
        status = extractJsonValue(body, "job_status");
    if (status.empty())
        status = extractJsonValue(body, "state");
    if (status.empty())
        return body;
    std::string lowered;
    lowered.reserve(status.size());
    for (char c : status)
        lowered.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    if (lowered == "complete" || lowered == "completed" || lowered == "success" ||
        lowered == "succeeded" || lowered == "done")
        return "completed";
    if (lowered == "failed" || lowered == "error" || lowered == "cancelled" ||
        lowered == "canceled")
        return "failed";
    return lowered;
}

std::string DiracClient::GetJobResult(const std::string& jobId) {
    std::string info = performRequest("/optimization/v1/jobs/" + jobId, "GET");
    std::string fileId = extractJsonValue(info, "file_id");
    if (!fileId.empty()) {
        return performRequest("/optimization/v1/files/" + fileId, "GET");
    }
    return info;
}

} // namespace quantum
} // namespace omnn
