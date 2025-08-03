
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
        
        std::cout << "[ResponderServer] Server is listening on TEST_TOPIC\n";
        
        // Keep the server running
        while (is_running()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void on_request(std::shared_ptr<network_message> req) override {
        if (!req || !req->is_request()) {
            std::cerr << "[ResponderServer] Invalid request received\n";
            return;
        }

        try {
            auto testReq = std::static_pointer_cast<test_request>(req);
            std::cout << "[ResponderServer] Received request ID: " << testReq->getId() << " Message: " << testReq->getMessage() << "\n";

            // Create response
            auto replyMsg = std::make_shared<test_reply>();
            replyMsg->setId(testReq->getId());
            replyMsg->setResponse("Response to: " + testReq->getMessage());
            replyMsg->setResponseTest(42);
            replyMsg->setRequest(*testReq);

            // Send reply immediately
            reply(req, replyMsg, "TEST_TOPIC", nullptr);
            std::cout << "[ResponderServer] Reply sent for request ID: " << testReq->getId() << "\n";
            
        } catch (const std::exception& e) {
            std::cerr << "[ResponderServer] Error processing request: " << e.what() << "\n";
            
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
        ResponderServer s(config);
        
        std::cout << "[ResponderServer] Starting responder server...\n";
        s.start();
        
    } catch (const std::exception& e) {
        std::cerr << "[ResponderServer] Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}