#include <network/youtube_blog_heartbeat.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>
#include <server/logger.h>

using namespace curious::net;
using namespace curious::log;

namespace curious::net {

void youtube_blog_heartbeat::toCapnp(curious::message::YoutubeBlogHeartbeat::Builder& builder) const {
    builder.setMsgType(curious::message::MessageType::YOUTUBE_BLOG_HEARTBEAT);

    builder.setTopic(_topic);
    builder.setResourcesCount(_resourcesCount);
}

youtube_blog_heartbeat youtube_blog_heartbeat::fromCapnp(const curious::message::YoutubeBlogHeartbeat::Reader& reader) {
    youtube_blog_heartbeat obj;

    obj._msgType = fromCapnpType(reader.getMsgType());
    obj._topic = reader.getTopic();
    obj._resourcesCount = reader.getResourcesCount();

    return obj;
}

std::string youtube_blog_heartbeat::serialize() const {
    ::capnp::MallocMessageBuilder message;
    auto builder = message.initRoot<curious::message::YoutubeBlogHeartbeat>();
    toCapnp(builder);

    kj::VectorOutputStream output;
    capnp::writeMessage(output, message);
    auto data = output.getArray();
    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

youtube_blog_heartbeat youtube_blog_heartbeat::deserialize(const std::string& data) {
    try {
        kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
        ::capnp::InputStreamMessageReader message(input);
        auto reader = message.getRoot<curious::message::YoutubeBlogHeartbeat>();
        return fromCapnp(reader);
    } catch (const std::exception& e) {
        LOG_ERR << "Failed to deserialize YoutubeBlogHeartbeat: " << e.what() << go;
        return youtube_blog_heartbeat{}; // Return default-constructed object
    }
}

}  // namespace curious::net
