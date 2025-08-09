
#include <server/server.h>
#include <videosd/videos_server.h>
#include <iostream>
#include <base/logger.h>

using namespace curious::core;
using namespace curious::net;

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " <config_path>\n";
            return 1;
        }

        std::string config_path = argv[1];
        server_config config(config_path);
        curious::videosd::video_server s(config, "videos_server");
        
        LOG_INFO << "[videos_server] Starting hybrid server..." << go;
        LOG_INFO << "[videos_server] This server both publishes and subscribes" << go;
        LOG_INFO << "[videos_server] Press Ctrl+C to stop" << go;
        
        s.start();
        
    } catch (const std::exception& e) {
        LOG_ERR << "[videos_server] Error: " << e.what() << go;
        return 1;
    }
    
    return 0;
}