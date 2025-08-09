#include <server/server.h>
#include <network/youtube_video_updates.h>

namespace curious::videosd {

using namespace curious::core;
using namespace curious::net;

class video_server : public server {
private:
    std::atomic<bool> shouldPublish{true};

public:
    using server::server;

    void run_loop() override;
    void publish_loop(const std::string& topic, int intervalMs);
    
    bool is_running() const {
        return _running;
    }
    
    void shutdown() {
        shouldPublish = false;
        stop();
    }
};

} // namespace curious::videosd