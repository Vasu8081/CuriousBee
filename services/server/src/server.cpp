// Complete Fixed server.cpp - All methods included with proper sync_listener

#include <server/server.h>
#include <network/network_message.h>
#include <network/factory_builder.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <future>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/io.h>
#include <filesystem>

namespace curious::core {

// Helper class for synchronous requests - implements all pure virtual methods
class sync_listener : public listener {
private:
    std::function<void(std::shared_ptr<curious::net::network_message>)> _callback;
    
public:
    sync_listener(std::function<void(std::shared_ptr<curious::net::network_message>)> callback)
        : _callback(std::move(callback)) {}
    
    void on_reply(std::shared_ptr<curious::net::network_message> reply) override {
        if (_callback) {
            _callback(reply);
        }
    }
    
    // Implement required pure virtual methods from listener interface
    void on_request(std::shared_ptr<curious::net::network_message> req) override {
        // sync_listener is only used for replies, so this shouldn't be called
        // But we need to implement it to make the class concrete
    }
    
    void on_message(std::shared_ptr<curious::net::network_message> msg) override {
        // sync_listener is only used for replies, so this shouldn't be called  
        // But we need to implement it to make the class concrete
    }
};

server::server(const server_config& config, const std::string& serverName)
    : _config(config), _running(false), _requestCounter(0), _serverName(serverName) {
    const auto& log_type = _config.get_log_type();
    if (log_type == "file") {
        _setup_file_logger(_config.get_log_file_path(), _config.get_timestamp_format());
    } else {
        _setup_console_logger();
    }
    _zmqContext = std::make_unique<zmq::context_t>(1);
}

server::~server() {
    stop();
}

void server::start() {
    if (_running) {
        LOG_INFO << "[server] Server is already running" << go;
        return;
    }
    
    _running = true;
    _listenerThread = std::thread(&server::_listener_loop, this);
    LOG_INFO << "[server] Server started" << go;
    run_loop();
}

void server::stop() {
    if (!_running) return;
    
    _running = false;
    
    // Wake up the listener thread
    if (_listenerThread.joinable()) {
        _listenerThread.join();
    }
    
    // Clean up sockets properly
    std::lock_guard<std::mutex> lock(_socketMutex);
    _pubSockets.clear();
    _subSockets.clear();
    _reqSockets.clear();
    _repSockets.clear();
    _reqSocketReady.clear();
    _requestReplySocketMap.clear();
    _pendingRequests.clear();
    
    LOG_INFO << "[server] Server stopped" << go;
}

void server::publish(std::shared_ptr<curious::net::network_message> msg, const std::string& topic) {
    if (!_running) {
        LOG_ERR << "[server] Cannot publish: server not running" << go;
        return;
    }
    _doPublish(std::move(msg), topic);
}

void server::request(std::shared_ptr<curious::net::network_message> req, const std::string& topic, 
                    std::shared_ptr<listener> callbackListener, void* closure, bool waitForReply) {
    if (!_running) {
        LOG_ERR << "[server] Cannot send request: server not running" << go;
        return;
    }
    
    if (waitForReply) {
        // For synchronous requests, use a condition variable to wait
        std::mutex waitMutex;
        std::condition_variable waitCondition;
        std::shared_ptr<curious::net::network_message> response = nullptr;
        bool replyReceived = false;
        
        // Create a synchronous callback listener
        auto syncListener = std::make_shared<sync_listener>(
            [&](std::shared_ptr<curious::net::network_message> reply) {
                std::lock_guard<std::mutex> lock(waitMutex);
                response = reply;
                replyReceived = true;
                waitCondition.notify_one();
            });
        
        _doRequest(std::move(req), topic, syncListener, closure, waitForReply);
        
        // Wait for reply with timeout
        std::unique_lock<std::mutex> lock(waitMutex);
        if (waitCondition.wait_for(lock, std::chrono::seconds(30), [&] { return replyReceived; })) {
            if (response && callbackListener) {
                callbackListener->on_reply(response);
            } else if (response) {
                on_reply(response);
            }
        } else {
            LOG_ERR << "[server] Request timed out waiting for reply" << go;
            if (callbackListener) {
                callbackListener->on_reply(nullptr);
            }
        }
    } else {
        _doRequest(std::move(req), topic, std::move(callbackListener), closure, waitForReply);
    }
}

// Async request with future
std::future<std::shared_ptr<curious::net::network_message>> 
server::request_async(std::shared_ptr<curious::net::network_message> req, const std::string& topic) {
    auto promise = std::make_shared<std::promise<std::shared_ptr<curious::net::network_message>>>();
    auto future = promise->get_future();
    
    if (!_running) {
        promise->set_exception(std::make_exception_ptr(
            std::runtime_error("Server not running")));
        return future;
    }
    
    // Create a callback listener that fulfills the promise
    auto callback = std::make_shared<promise_listener>(promise);
    _doRequest(std::move(req), topic, callback, nullptr, false);
    
    return future;
}

// Async request with callback
void server::request_async(std::shared_ptr<curious::net::network_message> req, const std::string& topic,
                          std::function<void(std::shared_ptr<curious::net::network_message>)> callback) {
    if (!_running) {
        LOG_ERR << "[server] Cannot send async request: server not running" << go;
        if (callback) {
            callback(nullptr);
        }
        return;
    }
    
    auto listener = std::make_shared<function_listener>(std::move(callback));
    _doRequest(std::move(req), topic, listener, nullptr, false);
}

void server::reply(std::shared_ptr<curious::net::network_message> req, 
                  std::shared_ptr<curious::net::network_message> resp, 
                  const std::string& topic, void* closure) {
    if (!_running) {
        LOG_ERR << "[server] Cannot reply: server not running " << go;
        return;
    }
    _doReply(req, resp, topic, closure);
}

void server::on_request(std::shared_ptr<curious::net::network_message> req) {
    LOG_INFO << "[server] [default] Request received" << go;
}

void server::on_reply(std::shared_ptr<curious::net::network_message> resp) {
    LOG_INFO << "[server] [default] Response received" << go;
}

void server::on_message(std::shared_ptr<curious::net::network_message> msg) {
    LOG_INFO << "[server] [default] Pub/Sub message received" << go;
}

void server::_doPublish(std::shared_ptr<curious::net::network_message> msg, const std::string& topic) {
    std::lock_guard<std::mutex> lock(_socketMutex);
    
    if (!msg) {
        LOG_ERR << "[server] Invalid message object" << go;
        return;
    }

    // Ensure PUB socket exists for the topic
    if (_pubSockets.find(topic) == _pubSockets.end()) {
        try {
            zmq::socket_t pub(*_zmqContext, zmq::socket_type::pub);
            std::string endpoint = _config.get_endpoint_for_topic(topic).endpoint;
            pub.bind(endpoint);
            _pubSockets[topic] = std::move(pub);
            LOG_INFO << "[server] Created PUB socket for topic: " << topic << " at " << endpoint << go;
        } catch (const zmq::error_t& e) {
            LOG_ERR << "[server] Failed to create PUB socket for topic " << topic << ": " << e.what() << go;
            return;
        }
    }

    try {
        capnp::MallocMessageBuilder builder;
        net::FactoryBuilder::toCapnp(builder, msg);

        kj::VectorOutputStream vecStream;
        writeMessage(vecStream, builder);

        auto& socket = _pubSockets[topic];
        zmq::message_t topicFrame(topic.begin(), topic.end());
        zmq::message_t dataFrame(vecStream.getArray().asChars().begin(), vecStream.getArray().size());

        socket.send(topicFrame, zmq::send_flags::sndmore);
        socket.send(dataFrame, zmq::send_flags::none);
        
        LOG_INFO << "[server] Published message on topic: " << topic << go;
    } catch (const std::exception& e) {
        LOG_ERR << "[server] Failed to publish message: " << e.what() << go;
    }
}

void server::_doReply(std::shared_ptr<curious::net::network_message> req, std::shared_ptr<curious::net::network_message> resp, const std::string& topic, void* /*closure*/) {
    if (!req || !resp || !resp->is_response() || !req->is_request()) {
        LOG_ERR << "[server] Invalid request or response objects" << go;
        return;
    }

    auto reqCast = std::dynamic_pointer_cast<curious::net::request>(req);
    auto respCast = std::dynamic_pointer_cast<curious::net::reply>(resp);
    if (!reqCast || !respCast) {
        LOG_ERR << "[server] Failed to cast request/response objects" << go;
        return;
    }
    
    respCast->setId(reqCast->getId());

    std::lock_guard<std::mutex> lock(_socketMutex);
    
    // Look up the socket from the request pointer
    auto it = _requestReplySocketMap.find(reqCast.get());
    if (it == _requestReplySocketMap.end() || it->second == nullptr) {
        LOG_ERR << "[server] No socket found for the given request" << go;
        return;
    }

    try {
        // Serialize and send the response
        capnp::MallocMessageBuilder builder;
        net::FactoryBuilder::toCapnp(builder, resp);

        kj::VectorOutputStream vecStream;
        writeMessage(vecStream, builder);

        zmq::message_t dataFrame(vecStream.getArray().asChars().begin(), vecStream.getArray().size());
        it->second->send(dataFrame, zmq::send_flags::none);
        
        LOG_INFO << "[server] Sent reply on topic: " << topic << " for request ID: " << reqCast->getId() << go;
    } catch (const zmq::error_t& err) {
        LOG_ERR << "[server] ZMQ send failed: " << err.what() << go;
    }

    // Clean up the mapping
    _requestReplySocketMap.erase(it);
}

void server::_doRequest(std::shared_ptr<curious::net::network_message> req, const std::string& topic, 
                       std::shared_ptr<listener> callbackListener, void* closure, bool waitForReply) {
    if (!req || !req->is_request()) {
        LOG_ERR << "[server] Invalid request object" << go;
        return;
    }

    auto reqPtr = std::dynamic_pointer_cast<curious::net::request>(req);
    if (!reqPtr) {
        LOG_ERR << "[server] Request is not of type request" << go;
        return;
    }

    std::unique_lock<std::mutex> lock(_socketMutex);
    
    // Assign unique request ID
    int id = ++_requestCounter;
    reqPtr->setId(id);

    // Create or recreate REQ socket to ensure clean state
    try {
        zmq::socket_t sock(*_zmqContext, zmq::socket_type::req);
        std::string endpoint = _config.get_endpoint_for_topic(topic).endpoint;
        
        // Set socket options for better reliability
        sock.set(zmq::sockopt::linger, 0); // Don't wait on close
        sock.set(zmq::sockopt::rcvtimeo, 30000); // 30 second receive timeout
        sock.set(zmq::sockopt::sndtimeo, 5000);  // 5 second send timeout
        
        sock.connect(endpoint);
        _reqSockets[topic] = std::move(sock);
        _reqSocketReady[topic] = true;
        
        LOG_INFO << "[server] Created/Reset REQ socket for topic: " << topic << " at " << endpoint << go;
    } catch (const zmq::error_t& e) {
        LOG_ERR << "[server] Failed to create REQ socket for topic " << topic << ": " << e.what() << go;
        return;
    }

    try {
        // Encode message
        capnp::MallocMessageBuilder builder;
        net::FactoryBuilder::toCapnp(builder, req);

        kj::VectorOutputStream vecStream;
        writeMessage(vecStream, builder);

        auto& socket = _reqSockets[topic];
        zmq::message_t dataFrame(vecStream.getArray().asChars().begin(), vecStream.getArray().size());
        
        socket.send(dataFrame, zmq::send_flags::none);
        _reqSocketReady[topic] = false; // Mark as waiting for reply
        
        LOG_INFO << "[server] Sent request ID: " << id << " to topic: " << topic << go;
        
        // Store pending request info
        _pendingRequests[id] = {callbackListener, closure, topic, std::chrono::steady_clock::now()};
        
    } catch (const std::exception& e) {
        LOG_ERR << "[server] Failed to send request: " << e.what() << go;
        _reqSocketReady[topic] = true; // Reset socket state on error
    }
}

void server::_listener_loop() {
    LOG_INFO << "[server] Listener thread started" << go;
    
    while (_running) {
        try {
            _handle_subscriber_messages();
            _handle_incoming_requests();
            _handle_request_replies();
            _cleanup_expired_requests();
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Reduced sleep for better responsiveness
        } catch (const std::exception& e) {
            LOG_ERR << "[server] Error in listener loop: " << e.what() << go;
        }
    }
    
    LOG_INFO << "[server] Listener thread stopped"<< go;
}

void server::_handle_subscriber_messages() {
    std::lock_guard<std::mutex> lock(_socketMutex);
    
    for (auto& [topic, socket] : _subSockets) {
        try {
            zmq::message_t topicFrame, dataFrame;
            if (!socket.recv(topicFrame, zmq::recv_flags::dontwait)) continue;
            if (!socket.recv(dataFrame, zmq::recv_flags::none)) continue;

            auto obj = _deserialize_message(dataFrame);
            if (!obj) continue;

            if (obj->is_request()) {
                on_request(obj);
            } else if (obj->is_response()) {
                on_reply(obj);
            } else {
                on_message(obj);
            }
        } catch (const std::exception& e) {
            LOG_ERR << "[server] Error handling subscriber message: " << e.what() << go;
        }
    }
}

void server::_handle_incoming_requests() {
    std::vector<std::pair<std::shared_ptr<curious::net::request>, zmq::socket_t*>> requestsToHandle;

    {
        std::lock_guard<std::mutex> lock(_socketMutex);
        
        for (auto& [topic, socket] : _repSockets) {
            try {
                zmq::message_t dataFrame;
                auto result = socket.recv(dataFrame, zmq::recv_flags::dontwait);
                if (!result) continue;

                auto obj = _deserialize_message(dataFrame);
                if (!obj || !obj->is_request()) {
                    // Send error response to maintain REQ/REP state
                    std::string errorMsg = "Invalid request";
                    zmq::message_t errorFrame(errorMsg.begin(), errorMsg.end());
                    socket.send(errorFrame, zmq::send_flags::none);
                    continue;
                }

                auto reqPtr = std::dynamic_pointer_cast<curious::net::request>(obj);
                if (!reqPtr) {
                    std::string errorMsg = "Failed to cast request";
                    zmq::message_t errorFrame(errorMsg.begin(), errorMsg.end());
                    socket.send(errorFrame, zmq::send_flags::none);
                    continue;
                }

                // Save the socket mapping and defer processing
                _requestReplySocketMap[reqPtr.get()] = &socket;
                requestsToHandle.emplace_back(reqPtr, &socket);

            } catch (const std::exception& e) {
                LOG_ERR << "[server] Error handling incoming request: " << e.what() << go;
                try {
                    std::string errorMsg = "Server error";
                    zmq::message_t errorFrame(errorMsg.begin(), errorMsg.end());
                    socket.send(errorFrame, zmq::send_flags::none);
                } catch (...) {
                    LOG_ERR << "[server] Failed to send error response, socket may be corrupted"<< go;
                }
            }
        }
    }

    // Call on_request outside of the mutex lock to avoid deadlock
    for (const auto& [reqPtr, _] : requestsToHandle) {
        on_request(reqPtr);
    }
}

void server::_handle_request_replies() {
    std::lock_guard<std::mutex> lock(_socketMutex);
    
    for (auto& [topic, socket] : _reqSockets) {
        // Only try to receive if we're expecting a reply
        if (_reqSocketReady[topic]) continue;
        
        try {
            zmq::message_t replyData;
            if (!socket.recv(replyData, zmq::recv_flags::dontwait)) continue;

            // Mark socket as ready for next request
            _reqSocketReady[topic] = true;

            auto response = _deserialize_message(replyData);
            if (!response || !response->is_response()) continue;

            auto respPtr = std::dynamic_pointer_cast<curious::net::reply>(response);
            if (!respPtr) continue;

            int id = respPtr->getId();
            LOG_INFO << "[server] Received reply for request ID: " << id << " on topic: " << topic << go;
            
            auto it = _pendingRequests.find(id);
            if (it != _pendingRequests.end()) {
                auto [callback, closure, reqTopic, timestamp] = it->second;
                _pendingRequests.erase(it);

                if (callback) {
                    callback->on_reply(respPtr);
                } else {
                    on_reply(respPtr);
                }
            } else {
                on_reply(respPtr);
            }
        } catch (const std::exception& e) {
            LOG_ERR << "[server] Error handling request reply: " << e.what() << go;
            _reqSocketReady[topic] = true; // Reset on error
        }
    }
}

void server::_cleanup_expired_requests() {
    const auto timeout = std::chrono::seconds(30); // 30 second timeout
    const auto now = std::chrono::steady_clock::now();
    
    std::lock_guard<std::mutex> lock(_socketMutex);
    
    for (auto it = _pendingRequests.begin(); it != _pendingRequests.end();) {
        auto [callback, closure, topic, timestamp] = it->second;
        if (now - timestamp > timeout) {
            LOG_ERR << "[server] Request ID " << it->first << " timed out"<< go;
            
            // Reset socket state for timed out requests
            _reqSocketReady[topic] = true;
            
            // Notify callback about timeout
            if (callback) {
                callback->on_reply(nullptr); // nullptr indicates timeout/error
            }
            
            it = _pendingRequests.erase(it);
        } else {
            ++it;
        }
    }
}

std::shared_ptr<curious::net::network_message> server::_deserialize_message(const zmq::message_t& frame) {
    try {
        kj::Array<capnp::word> wordArray = kj::heapArray<capnp::word>(
            (frame.size() + sizeof(capnp::word) - 1) / sizeof(capnp::word));
        std::memcpy(wordArray.begin(), frame.data(), frame.size());

        capnp::FlatArrayMessageReader reader(wordArray);
        return curious::net::FactoryBuilder::fromCapnp(reader);
    } catch (const std::exception& e) {
        LOG_ERR << "[server] Failed to deserialize message: " << e.what() << go;
        return nullptr;
    }
}

void server::listen(const std::string& topic) {
    const auto& endpointInfo = _config.get_endpoint_for_topic(topic);
    const auto& endpoint = endpointInfo.endpoint;

    if (endpoint.empty()) {
        LOG_ERR << "[server] No endpoint configured for topic: " << topic << go;
        return;
    }
    
    std::lock_guard<std::mutex> lock(_socketMutex);
    if (_repSockets.find(topic) != _repSockets.end()) {
        LOG_INFO << "[server] Already listening on topic: " << topic << go;
        return;
    }
    
    LOG_INFO << "[server] Listening on topic: " << topic << " at endpoint: " << endpoint << go;
    _activate_endpoint(endpointInfo, ActionType::Listen);
}

void server::subscribe(const std::string& topic) {
    const auto& endpointInfo = _config.get_endpoint_for_topic(topic);
    if (endpointInfo.endpoint.empty()) {
        LOG_ERR << "[server] No endpoint configured for topic: " << topic << go;
        return;
    }
    
    std::lock_guard<std::mutex> lock(_socketMutex);
    if (_subSockets.find(topic) != _subSockets.end()) {
        LOG_INFO << "[server] Already subscribed to topic: " << topic << go;
        return;
    }
    
    LOG_INFO << "[server] Subscribing to topic: " << topic << " at endpoint: " << endpointInfo.endpoint << go;
    _activate_endpoint(endpointInfo, ActionType::Subscribe);
}

void server::_activate_endpoint(messaging_endpoint endpointInfo, ActionType actionType) {
    try {
        switch (endpointInfo.type) {
            case EndpointType::TCP: {
                if (actionType == ActionType::Listen) {
                    zmq::socket_t rep(*_zmqContext, zmq::socket_type::rep);
                    
                    // Set socket options for better reliability
                    rep.set(zmq::sockopt::linger, 0);
                    rep.set(zmq::sockopt::rcvtimeo, -1); // Block indefinitely on receive
                    
                    rep.bind(endpointInfo.endpoint);
                    _repSockets[endpointInfo.topic] = std::move(rep);
                    LOG_INFO << "[server] Listening (TCP) on: " << endpointInfo.topic << " at " << endpointInfo.endpoint << go;
                } else if (actionType == ActionType::Subscribe) {
                    zmq::socket_t sub(*_zmqContext, zmq::socket_type::sub);
                    std::string connectEndpoint = endpointInfo.endpoint;
                    if (connectEndpoint.find("*") != std::string::npos)
                        connectEndpoint.replace(connectEndpoint.find("*"), 1, "127.0.0.1");

                    sub.connect(connectEndpoint);
                    sub.set(zmq::sockopt::subscribe, endpointInfo.topic);
                    _subSockets[endpointInfo.topic] = std::move(sub);

                    LOG_INFO << "[server] Subscribed (TCP) to: " << endpointInfo.topic << " at " << connectEndpoint << go;
                }
                break;
            }

            case EndpointType::IPC: {
                if (actionType == ActionType::Listen) {
                    zmq::socket_t rep(*_zmqContext, zmq::socket_type::rep);
                    rep.set(zmq::sockopt::linger, 0);
                    rep.bind(endpointInfo.endpoint);
                    _repSockets[endpointInfo.topic] = std::move(rep);
                    LOG_INFO << "[server] Listening (IPC) on: " << endpointInfo.topic << " at " << endpointInfo.endpoint << go;
                } else if (actionType == ActionType::Subscribe) {
                    zmq::socket_t sub(*_zmqContext, zmq::socket_type::sub);
                    sub.connect(endpointInfo.endpoint);
                    sub.set(zmq::sockopt::subscribe, endpointInfo.topic);
                    _subSockets[endpointInfo.topic] = std::move(sub);
                    LOG_INFO << "[server] Subscribed (IPC) to: " << endpointInfo.topic << " at " << endpointInfo.endpoint << go;
                }
                break;
            }

            default: {
                LOG_ERR << "[server] Unknown or unsupported endpoint type for topic: " << endpointInfo.topic << go;
                break;
            }
        }
    } catch (const zmq::error_t& e) {
        LOG_ERR << "[server] ZMQ error activating endpoint: " << e.what() << go;
    }
}

void server::_setup_console_logger() {
    try {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        console_sink->set_level(spdlog::level::debug);
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");  // Color on level

        auto logger = std::make_shared<spdlog::logger>("console_logger", console_sink);
        spdlog::set_default_logger(logger);
        spdlog::set_level(spdlog::level::debug);
        spdlog::flush_on(spdlog::level::info);

        LOG_INFO << "Console logger initialized with color output" << go;
    } catch (const spdlog::spdlog_ex& e) {
        std::cerr << "[server] Failed to setup console logger: " << e.what() << "\n";
    }
}

void server::_setup_file_logger(const std::string& path, const std::string&) {
    try {
        namespace fs = std::filesystem;

        std::string logFile = path + "/app_" + _serverName + ".log";
        if (!fs::exists(path)) {
            fs::create_directories(path);
        }

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile, true);

        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

        // Unique logger per server
        std::string logger_name = "multi_" + _serverName;

        // Remove existing logger with same name if present
        spdlog::drop(logger_name);

        auto multi_logger = std::make_shared<spdlog::logger>(logger_name, spdlog::sinks_init_list{console_sink, file_sink});
        spdlog::register_logger(multi_logger);
        spdlog::set_default_logger(multi_logger);

        spdlog::set_level(spdlog::level::debug);
        spdlog::flush_on(spdlog::level::info);

        LOG_INFO << "Logger initialized: " << logFile << go;
    } catch (const spdlog::spdlog_ex& e) {
        std::cerr << "[server] Failed to setup multi-sink logger: " << e.what() << "\n";
        _setup_console_logger();
    }
}

}  // namespace curious::core