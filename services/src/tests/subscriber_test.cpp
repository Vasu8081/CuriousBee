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
        LOG_INFO << "[SubscriberServer] Starting subscriber server..." << go;
        
        // Subscribe to multiple topics
        subscribe("TOPIC_A");
        subscribe("TOPIC_B");
        subscribe("NEWS_TOPIC");
        
        LOG_INFO << "[SubscriberServer] Subscribed to topics: TOPIC_A, TOPIC_B, NEWS_TOPIC" << go;
        LOG_INFO << "[SubscriberServer] Listening for messages..." << go;
        
        // Keep the server running and periodically show stats
        while (is_running()) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            print_statistics();
        }
    }

    void on_message(std::shared_ptr<network_message> msg) override {
        if (!msg) {
            LOG_ERR << "[SubscriberServer] Received null message" << go;
            return;
        }
        
        try {
            std::lock_guard<std::mutex> lock(counterMutex);
            
            if (msg->is_request()) {
                auto reqMsg = std::static_pointer_cast<test_request>(msg);
                std::string topic = determine_topic_from_content(reqMsg->getMessage());
                messageCounters[topic]++;
                
                LOG_INFO << "[SubscriberServer] [" << topic << "] Request received:" << go;
                LOG_INFO << "  ID: " << reqMsg->getId() << go;
                LOG_INFO << "  Message: " << reqMsg->getMessage() << go;
                LOG_INFO << "  User: " << reqMsg->getUser() << go;
                LOG_INFO << "  Age: " << reqMsg->getAge() << go;
                LOG_INFO << "  Total messages from " << topic << ": " << messageCounters[topic] << go;
                
            } else if (msg->is_response()) {
                auto respMsg = std::static_pointer_cast<test_reply>(msg);
                std::string topic = determine_topic_from_content(respMsg->getResponse());
                messageCounters[topic]++;
                
                LOG_INFO << "[SubscriberServer] [" << topic << "] Response received:" << go;
                LOG_INFO << "  ID: " << respMsg->getId() << go;
                LOG_INFO << "  Response: " << respMsg->getResponse() << go;
                LOG_INFO << "  ResponseTest: " << respMsg->getResponseTest() << go;
                LOG_INFO << "  Total messages from " << topic << ": " << messageCounters[topic] << go;
                
            } else {
                LOG_INFO << "[SubscriberServer] [UNKNOWN] Generic message received" << go;
                messageCounters["UNKNOWN"]++;
            }
            
        } catch (const std::exception& e) {
            LOG_ERR << "[SubscriberServer] Error processing message: " << e.what() << go;
        }
    }
    
    void on_request(std::shared_ptr<network_message> req) override {
        // This will be called for request-type messages in pub/sub
        LOG_INFO << "[SubscriberServer] Received request via pub/sub" << go;
        on_message(req);
    }
    
    void on_reply(std::shared_ptr<network_message> resp) override {
        // This will be called for response-type messages in pub/sub
        LOG_INFO << "[SubscriberServer] Received reply via pub/sub" << go;
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
        
        LOG_INFO << "\n=== MESSAGE STATISTICS ===" << go;
        for (const auto& [topic, count] : messageCounters) {
            LOG_INFO << topic << ": " << count << " messages" << go;
        }
        LOG_INFO << "==========================\n" << go;
    }
    
public:
    bool is_running() const {
        return _running; // Assuming _running is accessible or you add a getter
    }
};

int main() {
    try {
        server_config config("/home/curious_bytes/Documents/CuriousBee/services/config.json");
        SubscriberServer s(config, "SubscriberServer");
        
        LOG_INFO << "[SubscriberServer] Starting subscriber server..." << go;
        LOG_INFO << "[SubscriberServer] Will subscribe to topics: TOPIC_A, TOPIC_B, NEWS_TOPIC" << go;
        LOG_INFO << "[SubscriberServer] Press Ctrl+C to stop" << go;
        
        s.start();
        
    } catch (const std::exception& e) {
        LOG_ERR << "[SubscriberServer] Error: " << e.what() << go;
        return 1;
    }
    
    return 0;
}
