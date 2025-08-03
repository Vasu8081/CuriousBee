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
        LOG_INFO << "[HybridServer] Starting hybrid server (publisher + subscriber)..." << go;
        
        // Subscribe to topics we want to listen to
        subscribe("EXTERNAL_TOPIC");
        subscribe("BROADCAST_TOPIC");
        
        // Start publishing our own messages
        std::thread publishThread(&HybridServer::publish_loop, this);
        
        LOG_INFO << "[HybridServer] Server is running..." << go;
        LOG_INFO << "[HybridServer] Publishing on 'MY_TOPIC' every 4 seconds" << go;
        LOG_INFO << "[HybridServer] Subscribed to 'EXTERNAL_TOPIC' and 'BROADCAST_TOPIC'" << go;
        
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
                LOG_INFO << "[HybridServer] Published: " << msg->getMessage() << go;
                
            } catch (const std::exception& e) {
                LOG_ERR << "[HybridServer] Error publishing: " << e.what() << go;
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
                LOG_INFO << "[HybridServer] Received external request:" << go;
                LOG_INFO << "  ID: " << reqMsg->getId() << go;
                LOG_INFO << "  Message: " << reqMsg->getMessage() << go;
                LOG_INFO << "  From User: " << reqMsg->getUser() << go;
                
                // React to external messages by publishing our own response
                if (reqMsg->getMessage().find("important") != std::string::npos) {
                    react_to_important_message(reqMsg);
                }
                
            } else if (msg->is_response()) {
                auto respMsg = std::static_pointer_cast<test_reply>(msg);
                LOG_INFO << "[HybridServer] Received external response:" << go;
                LOG_INFO << "  ID: " << respMsg->getId() << go;
                LOG_INFO << "  Response: " << respMsg->getResponse() << go;
            }
            
        } catch (const std::exception& e) {
            LOG_ERR << "[HybridServer] Error processing received message: " << e.what() << go;
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
            LOG_INFO << "[HybridServer] Published reaction to important message" << go;
            
        } catch (const std::exception& e) {
            LOG_ERR << "[HybridServer] Error publishing reaction: " << e.what() << go;
        }
    }
    
    void print_status() {
        LOG_INFO << "\n[HybridServer] STATUS:" << go;
        LOG_INFO << "  Messages published: " << messageCounter.load() << go;
        LOG_INFO << "  Messages received: " << receivedMessages.load() << go;
        LOG_INFO << "  Server running: " << (is_running() ? "YES" : "NO") << go;
    }
    
public:
    bool is_running() const {
        return _running; // Assuming _running is accessible or you add a getter
    }
};

int main() {
    try {
        server_config config("/home/curious_bytes/Documents/CuriousBee/services/config.json");
        HybridServer s(config, "HybridServer");
        
        LOG_INFO << "[HybridServer] Starting hybrid server..." << go;
        LOG_INFO << "[HybridServer] This server both publishes and subscribes" << go;
        LOG_INFO << "[HybridServer] Press Ctrl+C to stop" << go;
        
        s.start();
        
    } catch (const std::exception& e) {
        LOG_ERR << "[HybridServer] Error: " << e.what() << go;
        return 1;
    }
    
    return 0;
}