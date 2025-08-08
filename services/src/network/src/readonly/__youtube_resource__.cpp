#include <network/youtube_resource.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>
#include <server/logger.h>

using namespace curious::net;
using namespace curious::log;

namespace curious::net {

void youtube_resource::toCapnp(curious::message::YoutubeResource::Builder& builder) const {
    builder.setMsgType(curious::message::MessageType::YOUTUBE_RESOURCE);

    builder.setTopic(_topic);
    builder.setResourceId(_resourceId);
    builder.setTitle(_title);
    builder.setData(_data);
    builder.setDescription(_description);
}

youtube_resource youtube_resource::fromCapnp(const curious::message::YoutubeResource::Reader& reader) {
    youtube_resource obj;

    obj._msgType = fromCapnpType(reader.getMsgType());
    obj._topic = reader.getTopic();
    obj._resourceId = reader.getResourceId();
    obj._title = reader.getTitle();
    obj._data = reader.getData();
    obj._description = reader.getDescription();

    return obj;
}

std::string youtube_resource::serialize() const {
    ::capnp::MallocMessageBuilder message;
    auto builder = message.initRoot<curious::message::YoutubeResource>();
    toCapnp(builder);

    kj::VectorOutputStream output;
    capnp::writeMessage(output, message);
    auto data = output.getArray();
    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

youtube_resource youtube_resource::deserialize(const std::string& data) {
    try {
        kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
        ::capnp::InputStreamMessageReader message(input);
        auto reader = message.getRoot<curious::message::YoutubeResource>();
        return fromCapnp(reader);
    } catch (const std::exception& e) {
        LOG_ERR << "Failed to deserialize YoutubeResource: " << e.what() << go;
        return youtube_resource{}; // Return default-constructed object
    }
}

}  // namespace curious::net
