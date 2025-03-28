#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <functional>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/strand.hpp>
#include "../../storage/CacheBase.h"

namespace omnn {
namespace rt {
namespace storage {

namespace fs = boost::filesystem;
namespace asio = boost::asio;

class RedisCache : public omnn::rt::storage::CacheBase {
public:
    RedisCache(const std::string& host = "localhost", int port = 6379,
               int timeout_ms = 1000, int retry_count = 5, int retry_delay_ms = 1000);
    ~RedisCache();

    // CacheBase interface implementation
    std::string GetOne(const std::string_view& key) override;
    bool Set(const std::string_view& key, const std::string_view& v) override;
    bool Clear(const std::string_view& key) override;
    bool ResetAllDB(const fs::path& path) override;

    // Additional Redis-specific methods
    bool IsConnected() const;

private:
    bool Connect();
    void Disconnect();
    bool RetryOperation(const std::function<bool()>& operation);
    
    // Execute Redis command and get response
    std::string ExecuteCommand(const std::string& command);
    
    // Redis connection using boost::asio
    asio::io_context io_context_;
    std::unique_ptr<asio::ip::tcp::socket> socket_;
    
    std::string host_;
    int port_;
    int timeout_ms_;
    int retry_count_;
    int retry_delay_ms_;
    bool connected_;
};

} // namespace storage
} // namespace rt
} // namespace omnn
