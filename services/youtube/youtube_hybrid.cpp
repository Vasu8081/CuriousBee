// Combined Publisher-Subscriber Server - Both publishes and subscribes

#include <server/server.h>
#include <network/test_request.h>
#include <network/test_reply.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <atomic>

using namespace curious::core;
using namespace curious::net;

class HybridServer : public server {
private:
    std::atomic<int> messageCounter{0};
    std::atomic<bool> shouldPublish{true};
    std::atomic<int> receivedMessages{0};

public:
    using server::server;

    void run_loop() override {
        std::cout << "[HybridServer] Starting hybrid server (publisher + subscriber)...\n";
        
        // Subscribe to topics we want to listen to
        subscribe("EXTERNAL_TOPIC");
        subscribe("BROADCAST_TOPIC");
        
        // Start publishing our own messages
        std::thread publishThread(&HybridServer::publish_loop, this);
        
        std::cout << "[HybridServer] Server is running...\n";
        std::cout << "[HybridServer] Publishing on 'MY_TOPIC' every 4 seconds\n";
        std::cout << "[HybridServer] Subscribed to 'EXTERNAL_TOPIC' and 'BROADCAST_TOPIC'\n";
        
        // Keep the server running
        while (is_running()) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            print_status();
        }
        
        shouldPublish = false;
        if (publishThread.joinable()) publishThread.join();
    }
    
    void publish_loop() {
        while (shouldPublish && is_running()) {
            try {
                int id = ++messageCounter;
                
                auto msg = std::make_shared<test_request>();
                msg->setId(id);
                msg->setMessage("Hybrid server message #" + std::to_string(id) + 
                              " - I can publish and subscribe!");
                msg->setUser("hybrid_server");
                msg->setAge(id);
                
                publish(msg, "MY_TOPIC");
                std::cout << "[HybridServer] Published: " << msg->getMessage() << "\n";
                
            } catch (const std::exception& e) {
                std::cerr << "[HybridServer] Error publishing: " << e.what() << "\n";
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(4));
        }
    }

    void on_message(std::shared_ptr<network_message> msg) override {
        if (!msg) return;
        
        receivedMessages++;
        
        try {
            if (msg->is_request()) {
                auto reqMsg = std::static_pointer_cast<test_request>(msg);
                std::cout << "[HybridServer] Received external request:\n";
                std::cout << "  ID: " << reqMsg->getId() << "\n";
                std::cout << "  Message: " << reqMsg->getMessage() << "\n";
                std::cout << "  From User: " << reqMsg->getUser() << "\n";
                
                // React to external messages by publishing our own response
                if (reqMsg->getMessage().find("important") != std::string::npos) {
                    react_to_important_message(reqMsg);
                }
                
            } else if (msg->is_response()) {
                auto respMsg = std::static_pointer_cast<test_reply>(msg);
                std::cout << "[HybridServer] Received external response:\n";
                std::cout << "  ID: " << respMsg->getId() << "\n";
                std::cout << "  Response: " << respMsg->getResponse() << "\n";
            }
            
        } catch (const std::exception& e) {
            std::cerr << "[HybridServer] Error processing received message: " << e.what() << "\n";
        }
    }
    
private:
    void react_to_important_message(std::shared_ptr<test_request> originalMsg) {
        try {
            auto reactionMsg = std::make_shared<test_reply>();
            reactionMsg->setId(++messageCounter);
            reactionMsg->setResponse("REACTION: I received an important message from " + 
                                   originalMsg->getUser() + "!");
            reactionMsg->setResponseTest(999);
            
            publish(reactionMsg, "REACTION_TOPIC");
            std::cout << "[HybridServer] Published reaction to important message\n";
            
        } catch (const std::exception& e) {
            std::cerr << "[HybridServer] Error publishing reaction: " << e.what() << "\n";
        }
    }
    
    void print_status() {
        std::cout << "\n[HybridServer] STATUS:\n";
        std::cout << "  Messages published: " << messageCounter.load() << "\n";
        std::cout << "  Messages received: " << receivedMessages.load() << "\n";
        std::cout << "  Server running: " << (is_running() ? "YES" : "NO") << "\n\n";
    }
    
public:
    bool is_running() const {
        return _running; // Assuming _running is accessible or you add a getter
    }
};

int main() {
    try {
        server_config config("/home/curious_bytes/Documents/CuriousBee/services/config.json");
        HybridServer s(config);
        
        std::cout << "[HybridServer] Starting hybrid server...\n";
        std::cout << "[HybridServer] This server both publishes and subscribes\n";
        std::cout << "[HybridServer] Press Ctrl+C to stop\n";
        
        s.start();
        
    } catch (const std::exception& e) {
        std::cerr << "[HybridServer] Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}