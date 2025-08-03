// Subscriber Server - Receives messages from topics

#include <server/server.h>
#include <network/test_request.h>
#include <network/test_reply.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <mutex>

using namespace curious::core;
using namespace curious::net;

class SubscriberServer : public server {
private:
    std::unordered_map<std::string, int> messageCounters;
    std::mutex counterMutex;
    
public:
    using server::server;

    void run_loop() override {
        std::cout << "[SubscriberServer] Starting subscriber server...\n";
        
        // Subscribe to multiple topics
        subscribe("TOPIC_A");
        subscribe("TOPIC_B");
        subscribe("NEWS_TOPIC");
        
        std::cout << "[SubscriberServer] Subscribed to topics: TOPIC_A, TOPIC_B, NEWS_TOPIC\n";
        std::cout << "[SubscriberServer] Listening for messages...\n";
        
        // Keep the server running and periodically show stats
        while (is_running()) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            print_statistics();
        }
    }

    void on_message(std::shared_ptr<network_message> msg) override {
        if (!msg) {
            std::cerr << "[SubscriberServer] Received null message\n";
            return;
        }
        
        try {
            std::lock_guard<std::mutex> lock(counterMutex);
            
            if (msg->is_request()) {
                auto reqMsg = std::static_pointer_cast<test_request>(msg);
                std::string topic = determine_topic_from_content(reqMsg->getMessage());
                messageCounters[topic]++;
                
                std::cout << "[SubscriberServer] [" << topic << "] Request received:\n";
                std::cout << "  ID: " << reqMsg->getId() << "\n";
                std::cout << "  Message: " << reqMsg->getMessage() << "\n";
                std::cout << "  User: " << reqMsg->getUser() << "\n";
                std::cout << "  Age: " << reqMsg->getAge() << "\n";
                std::cout << "  Total messages from " << topic << ": " << messageCounters[topic] << "\n\n";
                
            } else if (msg->is_response()) {
                auto respMsg = std::static_pointer_cast<test_reply>(msg);
                std::string topic = determine_topic_from_content(respMsg->getResponse());
                messageCounters[topic]++;
                
                std::cout << "[SubscriberServer] [" << topic << "] Response received:\n";
                std::cout << "  ID: " << respMsg->getId() << "\n";
                std::cout << "  Response: " << respMsg->getResponse() << "\n";
                std::cout << "  ResponseTest: " << respMsg->getResponseTest() << "\n";
                std::cout << "  Total messages from " << topic << ": " << messageCounters[topic] << "\n\n";
                
            } else {
                std::cout << "[SubscriberServer] [UNKNOWN] Generic message received\n";
                messageCounters["UNKNOWN"]++;
            }
            
        } catch (const std::exception& e) {
            std::cerr << "[SubscriberServer] Error processing message: " << e.what() << "\n";
        }
    }
    
    void on_request(std::shared_ptr<network_message> req) override {
        // This will be called for request-type messages in pub/sub
        std::cout << "[SubscriberServer] Received request via pub/sub\n";
        on_message(req);
    }
    
    void on_reply(std::shared_ptr<network_message> resp) override {
        // This will be called for response-type messages in pub/sub
        std::cout << "[SubscriberServer] Received reply via pub/sub\n";
        on_message(resp);
    }
    
private:
    std::string determine_topic_from_content(const std::string& content) {
        if (content.find("TOPIC_A") != std::string::npos) {
            return "TOPIC_A";
        } else if (content.find("TOPIC_B") != std::string::npos) {
            return "TOPIC_B";
        } else if (content.find("Breaking News") != std::string::npos || 
                   content.find("NEWS_TOPIC") != std::string::npos) {
            return "NEWS_TOPIC";
        }
        return "UNKNOWN";
    }
    
    void print_statistics() {
        std::lock_guard<std::mutex> lock(counterMutex);
        
        std::cout << "\n=== MESSAGE STATISTICS ===\n";
        for (const auto& [topic, count] : messageCounters) {
            std::cout << topic << ": " << count << " messages\n";
        }
        std::cout << "==========================\n\n";
    }
    
public:
    bool is_running() const {
        return _running; // Assuming _running is accessible or you add a getter
    }
};

int main() {
    try {
        server_config config("/home/curious_bytes/Documents/CuriousBee/services/config.json");
        SubscriberServer s(config);
        
        std::cout << "[SubscriberServer] Starting subscriber server...\n";
        std::cout << "[SubscriberServer] Will subscribe to topics: TOPIC_A, TOPIC_B, NEWS_TOPIC\n";
        std::cout << "[SubscriberServer] Press Ctrl+C to stop\n";
        
        s.start();
        
    } catch (const std::exception& e) {
        std::cerr << "[SubscriberServer] Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
