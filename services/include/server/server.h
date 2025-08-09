#pragma once

#include <zmq.hpp>
#include <memory>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <map>
#include <mutex>
#include <future>
#include <functional>
#include <chrono>
#include <network/network_message.h>
#include <server/server_config.h>
#include <server/listener.h>
#include <base/logger.h>

namespace curious::core {

enum class ActionType {
    Listen,
    Subscribe
};

class server {
public:
    explicit server(const server_config& config, const std::string& serverName);
    virtual ~server();

    // Core server control
    void start();
    void stop();
    virtual void run_loop() {}

    // Synchronous messaging
    void publish(std::shared_ptr<curious::net::network_message> msg, const std::string& topic);
    void request(std::shared_ptr<curious::net::network_message> req, const std::string& topic, 
                std::shared_ptr<listener> callbackListener = nullptr, void* closure = nullptr, 
                bool waitForReply = false);
    void reply(std::shared_ptr<curious::net::network_message> req, 
              std::shared_ptr<curious::net::network_message> resp, 
              const std::string& topic, void* closure = nullptr);

    // Asynchronous messaging
    std::future<std::shared_ptr<curious::net::network_message>> 
    request_async(std::shared_ptr<curious::net::network_message> req, const std::string& topic);
    
    void request_async(std::shared_ptr<curious::net::network_message> req, const std::string& topic,
                      std::function<void(std::shared_ptr<curious::net::network_message>)> callback);

    // Connection management
    void listen(const std::string& topic);
    void subscribe(const std::string& topic);

    // Event handlers (override in derived classes)
    virtual void on_request(std::shared_ptr<curious::net::network_message> req);
    virtual void on_reply(std::shared_ptr<curious::net::network_message> resp);
    virtual void on_message(std::shared_ptr<curious::net::network_message> msg);

protected:
    // Configuration and context
    server_config _config;
    std::unique_ptr<zmq::context_t> _zmqContext;
    
    // Threading
    std::atomic<bool> _running;
    std::thread _listenerThread;
    std::mutex _socketMutex;
    
    // Socket management
    std::unordered_map<std::string, zmq::socket_t> _pubSockets;
    std::unordered_map<std::string, zmq::socket_t> _subSockets;
    std::unordered_map<std::string, zmq::socket_t> _reqSockets;
    std::unordered_map<std::string, zmq::socket_t> _repSockets;
    std::unordered_map<std::string, bool> _reqSocketReady;
    
    // Request-reply mapping
    std::unordered_map<void*, zmq::socket_t*> _requestReplySocketMap;
    
    // Request tracking
    std::atomic<int> _requestCounter;
    struct PendingRequestInfo {
        std::shared_ptr<listener> callback;
        void* closure;
        std::string topic;
        std::chrono::steady_clock::time_point timestamp;
    };
    std::unordered_map<int, PendingRequestInfo> _pendingRequests;
    std::string _serverName;

private:
    // Core messaging implementations
    void _doPublish(std::shared_ptr<curious::net::network_message> msg, const std::string& topic);
    void _doRequest(std::shared_ptr<curious::net::network_message> req, const std::string& topic, 
                   std::shared_ptr<listener> callbackListener, void* closure, bool waitForReply);
    void _doReply(std::shared_ptr<curious::net::network_message> req, 
                 std::shared_ptr<curious::net::network_message> resp, 
                 const std::string& topic, void* closure);

    // Network loop and handlers
    void _listener_loop();
    void _handle_subscriber_messages();
    void _handle_incoming_requests();
    void _handle_request_replies();
    void _cleanup_expired_requests();
    
    // Utility functions
    std::shared_ptr<curious::net::network_message> _deserialize_message(const zmq::message_t& frame);
    void _activate_endpoint(messaging_endpoint endpointInfo, ActionType actionType);
    
    // Logging
    void _setup_console_logger();
    void _setup_file_logger(const std::string& path, const std::string& timeFormat);

    // Forward declarations for helper classes
    class promise_listener;
    class function_listener;
};


// Helper listener implementations
class server::promise_listener : public listener {
private:
    std::shared_ptr<std::promise<std::shared_ptr<curious::net::network_message>>> _promise;

public:
    explicit promise_listener(std::shared_ptr<std::promise<std::shared_ptr<curious::net::network_message>>> promise)
        : _promise(std::move(promise)) {}

    void on_reply(std::shared_ptr<curious::net::network_message> response) override {
        if (_promise) {
            _promise->set_value(response);
        }
    }

    // Implement required pure virtual methods with empty implementations
    void on_request(std::shared_ptr<curious::net::network_message> req) override {}
    void on_message(std::shared_ptr<curious::net::network_message> msg) override {}
};

class server::function_listener : public listener {
private:
    std::function<void(std::shared_ptr<curious::net::network_message>)> _callback;

public:
    explicit function_listener(std::function<void(std::shared_ptr<curious::net::network_message>)> callback)
        : _callback(std::move(callback)) {}

    void on_reply(std::shared_ptr<curious::net::network_message> response) override {
        if (_callback) {
            _callback(response);
        }
    }

    // Implement required pure virtual methods with empty implementations
    void on_request(std::shared_ptr<curious::net::network_message> req) override {}
    void on_message(std::shared_ptr<curious::net::network_message> msg) override {}
};

}  // namespace curious::core