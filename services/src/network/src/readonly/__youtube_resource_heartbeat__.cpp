#include <network/youtube_resource_heartbeat.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>
#include <base/logger.h>

using namespace curious::net;
using namespace curious::log;

namespace curious::net {

void youtube_resource_heartbeat::toCapnp(curious::message::YoutubeResourceHeartbeat::Builder& builder) const {
    builder.setMsgType(curious::message::MessageType::YOUTUBE_RESOURCE_HEARTBEAT);

    builder.setTopic(_topic);
    builder.setResourcesCount(_resourcesCount);
}

youtube_resource_heartbeat youtube_resource_heartbeat::fromCapnp(const curious::message::YoutubeResourceHeartbeat::Reader& reader) {
    youtube_resource_heartbeat obj;

    obj._msgType = fromCapnpType(reader.getMsgType());
    obj._topic = reader.getTopic();
    obj._resourcesCount = reader.getResourcesCount();

    return obj;
}

std::string youtube_resource_heartbeat::serialize() const {
    ::capnp::MallocMessageBuilder message;
    auto builder = message.initRoot<curious::message::YoutubeResourceHeartbeat>();
    toCapnp(builder);

    kj::VectorOutputStream output;
    capnp::writeMessage(output, message);
    auto data = output.getArray();
    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

youtube_resource_heartbeat youtube_resource_heartbeat::deserialize(const std::string& data) {
    try {
        kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
        ::capnp::InputStreamMessageReader message(input);
        auto reader = message.getRoot<curious::message::YoutubeResourceHeartbeat>();
        return fromCapnp(reader);
    } catch (const std::exception& e) {
        LOG_ERR << "Failed to deserialize YoutubeResourceHeartbeat: " << e.what() << go;
        return youtube_resource_heartbeat{}; // Return default-constructed object
    }
}

}  // namespace curious::net
