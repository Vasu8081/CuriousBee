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
        std::cout << "[RequesterServer] Finished sending test requests\n";
    }
    
    void send_test_request(int requestNum) {
        replyReceived = false;
        lastReply = nullptr;
        
        auto msg = std::make_shared<test_request>();
        msg->setId(requestNum);
        msg->setAge(30 + requestNum);
        msg->setMessage("Hello, World request #" + std::to_string(requestNum));
        msg->setUser("test_user_" + std::to_string(requestNum));

        std::cout << "[RequesterServer] Sending request #" << requestNum << ": " << msg->getMessage() << "\n";
        
        // Send synchronous request (waitForReply = true)
        request(msg, "TEST_TOPIC", nullptr, nullptr, true);
        
        std::cout << "[RequesterServer] Request #" << requestNum << " completed\n";
    }

    void on_reply(std::shared_ptr<curious::net::network_message> response) override {
        std::lock_guard<std::mutex> lock(replyMutex);
        
        if (!response) {
            std::cerr << "[RequesterServer] Received null response (timeout or error)\n";
            replyReceived = true;
            return;
        }
        
        if (!response->is_response()) {
            std::cerr << "[RequesterServer] Invalid response received\n";
            replyReceived = true;
            return;
        }

        auto respCast = std::dynamic_pointer_cast<curious::net::test_reply>(response);
        if (!respCast) {
            std::cerr << "[RequesterServer] Failed to cast to test_reply\n";
            replyReceived = true;
            return;
        }

        lastReply = respCast;
        replyReceived = true;
        
        std::cout << "[RequesterServer] Reply received!\n";
        std::cout << "  ID: " << respCast->getId() << "\n";
        std::cout << "  Response: " << respCast->getResponse() << "\n";
        std::cout << "  ResponseTest: " << respCast->getResponseTest() << "\n";
    }
};

int main() {
    try {
        server_config config("/home/curious_bytes/Documents/CuriousBee/services/config.json");
        RequesterServer s(config);
        
        std::cout << "[RequesterServer] Starting requester server...\n";
        s.start();
        
    } catch (const std::exception& e) {
        std::cerr << "[RequesterServer] Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}