// Fixed RequesterServer - Properly waits for replies

#include <server/server.h>
#include <network/test_request.h>
#include <network/test_reply.h>
#include <thread>
#include <chrono>
#include <iostream>

using namespace curious::core;
using namespace curious::net;

class RequesterServer : public server {
private:
    std::atomic<bool> replyReceived{false};
    std::shared_ptr<test_reply> lastReply;
    std::mutex replyMutex;

public:
    using server::server;

    void run_loop() override {
        // Send multiple requests to test reliability
        for (int i = 1; i <= 3; ++i) {
            send_test_request(i);
            
            // Wait a bit between requests
            if (i < 3) {
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }
        
        // Keep running for a bit to ensure all replies are processed
        std::this_thread::sleep_for(std::chrono::seconds(5));
        LOG_INFO << "[RequesterServer] Finished sending test requests" << go;
    }
    
    void send_test_request(int requestNum) {
        replyReceived = false;
        lastReply = nullptr;
        
        auto msg = std::make_shared<test_request>();
        msg->setId(requestNum);
        msg->setAge(30 + requestNum);
        msg->setMessage("Hello, World request #" + std::to_string(requestNum));
        msg->setUser("test_user_" + std::to_string(requestNum));

        LOG_INFO << "[RequesterServer] Sending request #" << requestNum << ": " << msg->getMessage() << go;
        
        // Send synchronous request (waitForReply = true)
        request(msg, "TEST_TOPIC", nullptr, nullptr, true);
        
        LOG_INFO << "[RequesterServer] Request #" << requestNum << " completed" << go;
    }

    void on_reply(std::shared_ptr<curious::net::network_message> response) override {
        std::lock_guard<std::mutex> lock(replyMutex);
        
        if (!response) {
            LOG_ERR << "[RequesterServer] Received null response (timeout or error)" << go;
            replyReceived = true;
            return;
        }
        
        if (!response->is_response()) {
            LOG_ERR << "[RequesterServer] Invalid response received" << go;
            replyReceived = true;
            return;
        }

        auto respCast = std::dynamic_pointer_cast<curious::net::test_reply>(response);
        if (!respCast) {
            LOG_ERR << "[RequesterServer] Failed to cast to test_reply" << go;
            replyReceived = true;
            return;
        }

        lastReply = respCast;
        replyReceived = true;
        
        LOG_INFO << "[RequesterServer] Reply received!" << go;
        LOG_INFO << "  ID: " << respCast->getId() << go;
        LOG_INFO << "  Response: " << respCast->getResponse() << go;
        LOG_INFO << "  ResponseTest: " << respCast->getResponseTest() << go;
    }
};

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " <config_path>\n";
            return 1;
        }

        std::string config_path = argv[1];
        server_config config(config_path);

        RequesterServer s(config, "RequesterServer");

        LOG_INFO << "[RequesterServer] Starting requester server..." << go;
        s.start();

    } catch (const std::exception& e) {
        LOG_ERR << "[RequesterServer] Error: " << e.what() << go;
        return 1;
    }

    return 0;
}