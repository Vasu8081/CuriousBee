
// Publisher Server - Sends messages on topics

#include <server/server.h>
#include <network/test_request.h>
#include <network/test_reply.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <atomic>

using namespace curious::core;
using namespace curious::net;

class PublisherServer : public server {
private:
    std::atomic<int> messageCounter{0};
    std::atomic<bool> shouldPublish{true};

public:
    using server::server;

    void run_loop() override {
        LOG_INFO << "[PublisherServer] Starting publisher server..." << go;
        
        // Start publishing on multiple topics
        std::thread publishThread1(&PublisherServer::publish_loop, this, "TOPIC_A", 2000); // 2 seconds
        std::thread publishThread2(&PublisherServer::publish_loop, this, "TOPIC_B", 3000); // 3 seconds
        std::thread publishThread3(&PublisherServer::publish_loop, this, "NEWS_TOPIC", 5000); // 5 seconds
        
        // Keep the server running
        while (is_running()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        shouldPublish = false;
        
        if (publishThread1.joinable()) publishThread1.join();
        if (publishThread2.joinable()) publishThread2.join();
        if (publishThread3.joinable()) publishThread3.join();
    }
    
    void publish_loop(const std::string& topic, int intervalMs) {
        LOG_INFO << "[PublisherServer] Starting publish loop for topic: " << topic 
                  << " (interval: " << intervalMs << "ms)" << go;
        
        int topicCounter = 0;
        
        while (shouldPublish && is_running()) {
            try {
                topicCounter++;
                int globalId = ++messageCounter;
                
                if (topic == "NEWS_TOPIC") {
                    // Publish news-style messages
                    auto newsMsg = std::make_shared<test_request>();
                    newsMsg->setId(globalId);
                    newsMsg->setMessage("Breaking News #" + std::to_string(topicCounter) + 
                                      " - Important update at " + get_current_time());
                    newsMsg->setUser("news_publisher");
                    newsMsg->setAge(topicCounter);
                    
                    publish(newsMsg, topic);
                    LOG_INFO << "[PublisherServer] Published NEWS: " << newsMsg->getMessage() << go;
                    
                } else {
                    // Publish regular messages
                    auto msg = std::make_shared<test_reply>();
                    msg->setId(globalId);
                    msg->setResponse("Message from " + topic + " #" + std::to_string(topicCounter) + 
                                   " at " + get_current_time());
                    msg->setResponseTest(topicCounter * 10);
                    
                    publish(msg, topic);
                    LOG_INFO << "[PublisherServer] Published on " << topic << ": " << msg->getResponse() << go;
                }
                
            } catch (const std::exception& e) {
                LOG_ERR << "[PublisherServer] Error publishing on " << topic << ": " << e.what() << go;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
        }
        
        LOG_INFO << "[PublisherServer] Stopped publishing on topic: " << topic << go;
    }
    
    std::string get_current_time() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&time_t);
        
        char buffer[100];
        std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &tm);
        return std::string(buffer);
    }
    
    bool is_running() const {
        return _running; // Assuming _running is accessible or you add a getter
    }
    
    void shutdown() {
        shouldPublish = false;
        stop();
    }
};

int main() {
    try {
        server_config config("/home/curious_bytes/Documents/CuriousBee/services/config.json");
        PublisherServer s(config, "PublisherServer");
        
        LOG_INFO << "[PublisherServer] Starting publisher server..." << go;
        LOG_INFO << "[PublisherServer] Will publish on topics: TOPIC_A, TOPIC_B, NEWS_TOPIC" << go;
        LOG_INFO << "[PublisherServer] Press Ctrl+C to stop" << go;
        
        s.start();
        
    } catch (const std::exception& e) {
        LOG_ERR << "[PublisherServer] Error: " << e.what() << go;
        return 1;
    }
    
    return 0;
}