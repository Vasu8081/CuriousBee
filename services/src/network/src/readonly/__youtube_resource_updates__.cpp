#include <network/youtube_resource_updates.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>
#include <base/logger.h>

using namespace curious::net;
using namespace curious::log;

namespace curious::net {

void youtube_resource_updates::toCapnp(curious::message::YoutubeResourceUpdates::Builder& builder) const {
    builder.setMsgType(curious::message::MessageType::YOUTUBE_RESOURCE_UPDATES);

    builder.setTopic(_topic);
    // Serialize list field: updates
    auto UpdatesList = builder.initUpdates(_updates.size());
    for (size_t i = 0; i < _updates.size(); ++i) {
        auto itemBuilder = UpdatesList[i];
        _updates[i].toCapnp(itemBuilder);
    }

}

youtube_resource_updates youtube_resource_updates::fromCapnp(const curious::message::YoutubeResourceUpdates::Reader& reader) {
    youtube_resource_updates obj;

    obj._msgType = fromCapnpType(reader.getMsgType());
    obj._topic = reader.getTopic();
    // Deserialize list field: updates
    auto UpdatesList = reader.getUpdates();
    obj._updates.clear();
    obj._updates.reserve(UpdatesList.size());
    for (const auto& item : UpdatesList) {
        obj._updates.push_back(youtube_resource::fromCapnp(item));
    }


    return obj;
}

std::string youtube_resource_updates::serialize() const {
    ::capnp::MallocMessageBuilder message;
    auto builder = message.initRoot<curious::message::YoutubeResourceUpdates>();
    toCapnp(builder);

    kj::VectorOutputStream output;
    capnp::writeMessage(output, message);
    auto data = output.getArray();
    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

youtube_resource_updates youtube_resource_updates::deserialize(const std::string& data) {
    try {
        kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
        ::capnp::InputStreamMessageReader message(input);
        auto reader = message.getRoot<curious::message::YoutubeResourceUpdates>();
        return fromCapnp(reader);
    } catch (const std::exception& e) {
        LOG_ERR << "Failed to deserialize YoutubeResourceUpdates: " << e.what() << go;
        return youtube_resource_updates{}; // Return default-constructed object
    }
}

}  // namespace curious::net
