#include <network/youtube_video_heartbeat.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>
#include <server/logger.h>

using namespace curious::net;
using namespace curious::log;

namespace curious::net {

void youtube_video_heartbeat::toCapnp(curious::message::YoutubeVideoHeartbeat::Builder& builder) const {
    builder.setMsgType(curious::message::MessageType::YOUTUBE_VIDEO_HEARTBEAT);

    builder.setTopic(_topic);
    builder.setVideosCount(_videosCount);
}

youtube_video_heartbeat youtube_video_heartbeat::fromCapnp(const curious::message::YoutubeVideoHeartbeat::Reader& reader) {
    youtube_video_heartbeat obj;

    obj._msgType = fromCapnpType(reader.getMsgType());
    obj._topic = reader.getTopic();
    obj._videosCount = reader.getVideosCount();

    return obj;
}

std::string youtube_video_heartbeat::serialize() const {
    ::capnp::MallocMessageBuilder message;
    auto builder = message.initRoot<curious::message::YoutubeVideoHeartbeat>();
    toCapnp(builder);

    kj::VectorOutputStream output;
    capnp::writeMessage(output, message);
    auto data = output.getArray();
    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

youtube_video_heartbeat youtube_video_heartbeat::deserialize(const std::string& data) {
    try {
        kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
        ::capnp::InputStreamMessageReader message(input);
        auto reader = message.getRoot<curious::message::YoutubeVideoHeartbeat>();
        return fromCapnp(reader);
    } catch (const std::exception& e) {
        LOG_ERR << "Failed to deserialize YoutubeVideoHeartbeat: " << e.what() << go;
        return youtube_video_heartbeat{}; // Return default-constructed object
    }
}

}  // namespace curious::net
