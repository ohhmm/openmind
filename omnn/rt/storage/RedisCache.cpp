#include "RedisCache.h"
#include <chrono>
#include <thread>
#include <stdexcept>
#include <sstream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/error_code.hpp>

namespace omnn {
namespace rt {
namespace storage {

RedisCache::RedisCache(const std::string& host, int port, int timeout_ms,
                     int retry_count, int retry_delay_ms)
    : host_(host), port_(port), timeout_ms_(timeout_ms),
      retry_count_(retry_count), retry_delay_ms_(retry_delay_ms),
      connected_(false) {
    
    if (!Connect()) {
        throw std::runtime_error("Failed to connect to Redis server");
    }
}

RedisCache::~RedisCache() {
    Disconnect();
}

bool RedisCache::Connect() {
    Disconnect();  // Ensure any existing connection is closed

    try {
        socket_ = std::make_unique<boost::asio::ip::tcp::socket>(io_context_);
        
        boost::asio::ip::tcp::resolver resolver(io_context_);
        boost::asio::ip::tcp::resolver::query query(host_, std::to_string(port_));
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        
        boost::system::error_code ec;
        boost::asio::connect(*socket_, endpoint_iterator, ec);
        
        if (ec) {
            connected_ = false;
            return false;
        }
        
        connected_ = true;
        return true;
    } catch (const std::exception&) {
        connected_ = false;
        return false;
    }
}

void RedisCache::Disconnect() {
    if (socket_ && socket_->is_open()) {
        boost::system::error_code ec;
        socket_->close(ec);
        socket_.reset();
    }
    connected_ = false;
}

bool RedisCache::RetryOperation(const std::function<bool()>& operation) {
    for (int i = 0; i < retry_count_; ++i) {
        if (operation()) {
            return true;
        }

        if (!IsConnected() && !Connect()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(retry_delay_ms_));
            continue;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(retry_delay_ms_));
    }
    return false;
}

std::string RedisCache::ExecuteCommand(const std::string& command) {
    if (!IsConnected()) {
        return "";
    }
    
    try {
        boost::system::error_code ec;
        boost::asio::write(*socket_, boost::asio::buffer(command), ec);
        
        if (ec) {
            return "";
        }
        
        boost::asio::streambuf response;
        boost::asio::read_until(*socket_, response, "\r\n", ec);
        
        if (ec) {
            return "";
        }
        
        std::istream response_stream(&response);
        std::string response_line;
        std::getline(response_stream, response_line);
        
        if (response_line.empty()) {
            return "";
        }
        
        char type = response_line[0];
        std::string value = response_line.substr(1);
        
        switch (type) {
            case '+': // Simple string
                return value;
            case '-': // Error
                return "";
            case ':': // Integer
                return value;
            case '$': { // Bulk string
                int length = std::stoi(value);
                if (length == -1) {
                    return ""; // Null response
                }
                
                std::string bulk_string;
                bulk_string.resize(length);
                
                boost::asio::streambuf buffer;
                size_t bytes_read = boost::asio::read(*socket_, 
                    boost::asio::buffer(&bulk_string[0], length), ec);
                
                if (ec || bytes_read != length) {
                    return "";
                }
                
                boost::asio::read_until(*socket_, buffer, "\r\n", ec);
                if (ec) {
                    return "";
                }
                
                boost::asio::read_until(*socket_, response, "\r\n", ec);
                if (ec) {
                    return "";
                }
                
                return bulk_string;
            }
            default:
                return "";
        }
    } catch (const std::exception&) {
        return "";
    }
}

std::string RedisCache::GetOne(const std::string_view& key) {
    std::string result;
    
    RetryOperation([this, &key, &result]() {
        if (!IsConnected()) return false;
        
        std::stringstream cmd;
        cmd << "*2\r\n$3\r\nGET\r\n$" << key.size() << "\r\n" << key << "\r\n";
        
        result = ExecuteCommand(cmd.str());
        return !result.empty() || result == ""; // Empty string is a valid result for non-existent key
    });
    
    return result;
}

bool RedisCache::Set(const std::string_view& key, const std::string_view& v) {
    return RetryOperation([this, &key, &v]() {
        if (!IsConnected()) return false;
        
        std::stringstream cmd;
        cmd << "*3\r\n$3\r\nSET\r\n$" << key.size() << "\r\n" << key 
            << "\r\n$" << v.size() << "\r\n" << v << "\r\n";
        
        std::string result = ExecuteCommand(cmd.str());
        return result == "OK";
    });
}

bool RedisCache::Clear(const std::string_view& key) {
    return RetryOperation([this, &key]() {
        if (!IsConnected()) return false;
        
        std::stringstream cmd;
        cmd << "*2\r\n$3\r\nDEL\r\n$" << key.size() << "\r\n" << key << "\r\n";
        
        std::string result = ExecuteCommand(cmd.str());
        return !result.empty(); // Any non-empty result indicates success
    });
}

bool RedisCache::ResetAllDB(const fs::path& path) {
    CacheBase::ResetAllDB(path);
    
    return RetryOperation([this]() {
        if (!IsConnected()) return false;
        
        std::string cmd = "*1\r\n$7\r\nFLUSHDB\r\n";
        
        std::string result = ExecuteCommand(cmd);
        return result == "OK";
    });
}

bool RedisCache::IsConnected() const {
    return connected_ && socket_ && socket_->is_open();
}

} // namespace storage
} // namespace rt
} // namespace omnn
