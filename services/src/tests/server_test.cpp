
// Fixed ResponderServer - Improved error handling and socket management

#include <server/server.h>
#include <network/test_request.h>
#include <network/test_reply.h>
#include <thread>
#include <chrono>
#include <iostream>

using namespace curious::core;
using namespace curious::net;

class ResponderServer : public server {
public:
    using server::server;

    void run_loop() override {
        listen("TEST_TOPIC");
        
        LOG_INFO << "[ResponderServer] Server is listening on TEST_TOPIC" << go;
        
        // Keep the server running
        while (is_running()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void on_request(std::shared_ptr<network_message> req) override {
        if (!req || !req->is_request()) {
            LOG_ERR << "[ResponderServer] Invalid request received" << go;
            return;
        }

        try {
            auto testReq = std::static_pointer_cast<test_request>(req);
            LOG_INFO << "[ResponderServer] Received request ID: " << testReq->getId() << " Message: " << testReq->getMessage() << go;

            // Create response
            auto replyMsg = std::make_shared<test_reply>();
            replyMsg->setId(testReq->getId());
            replyMsg->setResponse("Response to: " + testReq->getMessage());
            replyMsg->setResponseTest(42);
            replyMsg->setRequest(*testReq);

            // Send reply immediately
            reply(req, replyMsg, "TEST_TOPIC", nullptr);
            LOG_INFO << "[ResponderServer] Reply sent for request ID: " << testReq->getId() << go;
            
        } catch (const std::exception& e) {
            LOG_ERR << "[ResponderServer] Error processing request: " << e.what() << go;
            
            // Send error response to maintain REQ/REP state
            auto errorReply = std::make_shared<test_reply>();
            if (req->is_request()) {
                auto reqCast = std::static_pointer_cast<test_request>(req);
                errorReply->setId(reqCast->getId());
            }
            errorReply->setResponse("Error processing request");
            errorReply->setResponseTest(-1);
            
            reply(req, errorReply, "TEST_TOPIC", nullptr);
        }
    }
    
    // Add method to check if server is running (you'll need to add this to base class)
    bool is_running() const {
        return _running; // Assuming _running is accessible or you add a getter
    }
};

int main() {
    try {
        server_config config("/home/curious_bytes/Documents/CuriousBee/services/config.json");
        ResponderServer s(config, "ResponderServer");
        
        LOG_INFO << "[ResponderServer] Starting responder server..." << go;
        s.start();
        
    } catch (const std::exception& e) {
        LOG_ERR << "[ResponderServer] Error: " << e.what() << go;
        return 1;
    }
    
    return 0;
}