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
#include <stdexcept>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

namespace omnn {
namespace quantum {

DiracClient::DiracClient() {
    const char* token = std::getenv("QCI_TOKEN");
    const char* url = std::getenv("QCI_API_URL");

    if (!token || !url) {
        throw std::runtime_error("QCI_TOKEN and QCI_API_URL environment variables must be set");
    }

    _token = token;
    _apiUrl = url;
}

std::string DiracClient::performRequest(const std::string& endpoint,
                                       const std::string& method,
                                       const std::string& payload) {
    // Parse URL (assuming https://host/path)
    std::string host;
    std::string path = endpoint;

    std::string url = _apiUrl;
    size_t protocol_pos = url.find("://");
    if (protocol_pos != std::string::npos) {
        url = url.substr(protocol_pos + 3);
    }

    size_t host_end = url.find("/");
    if (host_end == std::string::npos) {
        host = url;
    } else {
        host = url.substr(0, host_end);
        // If _apiUrl has a path, prepend it to endpoint
        std::string base_path = url.substr(host_end);
        path = base_path + endpoint;
    }

    // Networking setup
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

    // Request setup
    http::request<http::string_body> req;
    req.version(11);
    req.method(method == "POST" ? http::verb::post : (method == "PUT" ? http::verb::put : http::verb::get));
    req.target(path);
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::authorization, "Bearer " + _token);
    req.set(http::field::content_type, "application/json");

    if (!payload.empty()) {
        req.body() = payload;
        req.prepare_payload();
    }

    http::write(stream, req);

    // Response setup
    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);

    // Graceful shutdown
    beast::error_code ec;
    stream.shutdown(ec);

    if (res.result() != http::status::ok) {
        throw std::runtime_error("API request failed with status " + std::to_string(static_cast<int>(res.result())) + ": " + res.body());
    }

    return res.body();
}

std::string DiracClient::UploadProblem(const std::string& jsonProblem) {
    return performRequest("/upload", "POST", jsonProblem);
}

std::string DiracClient::CreateJob(const std::string& problemId, const std::string& device) {
    std::string payload = "{\"problem_id\": \"" + problemId + "\", \"device_type\": \"" + device + "\"}";
    return performRequest("/jobs", "POST", payload);
}

std::string DiracClient::PollJobStatus(const std::string& jobId) {
    return performRequest("/jobs/" + jobId, "GET");
}

std::string DiracClient::GetJobResult(const std::string& jobId) {
    return performRequest("/jobs/" + jobId + "/result", "GET");
}

} // namespace quantum
} // namespace omnn
