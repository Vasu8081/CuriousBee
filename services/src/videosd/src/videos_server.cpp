#include <videosd/videos_server.h>

using namespace curious::videosd;

void video_server::run_loop()  {
    LOG_INFO << "[video_server] Starting publisher server..." << go;
    std::thread publishThread1(&video_server::publish_loop, this, "YOUTUBE_VIDEO_UPDATE", 60000); // 2 minutes
    
    while (is_running()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    shouldPublish = false;
    
    if (publishThread1.joinable()) publishThread1.join();
}

void video_server::publish_loop(const std::string& topic, int intervalMs) {
    LOG_INFO << "[video_server] Starting publish loop for topic: " << topic << " (interval: " << intervalMs << "ms)" << go;
    int topicCounter = 0;
    
    while (shouldPublish && is_running()) {
        try {
            topicCounter++;
            
            if (topic == "YOUTUBE_VIDEO_UPDATE") {
                // Publish news-style messages
                auto update = std::make_shared<youtube_video_updates>();
                update->setMsgType(message_type::youtubeVideoUpdates);
                update->setTopic(topic);

                std::vector<youtube_video> videos;
                // Add some dummy video data
                for (int i = 0; i < 5; ++i) {
                    youtube_video video;
                    video.setVideoId("video_" + std::to_string(topicCounter) + "_" + std::to_string(i));
                    video.setTitle("Video Title " + std::to_string(topicCounter) + " - " + std::to_string(i));
                    video.setThumbnail("http://example.com/thumbnail_" + std::to_string(i) + ".jpg");
                    videos.push_back(video);
                }
                update->setVideos(videos);
                
                publish(update, topic);
                LOG_INFO << "[video_server] Published " << update->getVideos().size() << " videos on topic: " << topic << go;
            }
        } catch (const std::exception& e) {
            LOG_ERR << "[video_server] Error publishing on " << topic << ": " << e.what() << go;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
    }
    
    LOG_INFO << "[video_server] Stopped publishing on topic: " << topic << go;
}