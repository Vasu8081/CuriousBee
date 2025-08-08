#include <network/youtube_resource_snapshot_response.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>
#include <server/logger.h>

using namespace curious::net;
using namespace curious::log;

namespace curious::net {

void youtube_resource_snapshot_response::toCapnp(curious::message::YoutubeResourceSnapshotResponse::Builder& builder) const {
    builder.setMsgType(curious::message::MessageType::YOUTUBE_RESOURCE_SNAPSHOT_RESPONSE);

    builder.setTopic(_topic);
    builder.setId(_id);
    // Serialize custom message field: request
    auto RequestBuilder = builder.initRequest();
    _request.toCapnp(RequestBuilder);

    // Serialize list field: resources
    auto ResourcesList = builder.initResources(_resources.size());
    for (size_t i = 0; i < _resources.size(); ++i) {
        auto itemBuilder = ResourcesList[i];
        _resources[i].toCapnp(itemBuilder);
    }

}

youtube_resource_snapshot_response youtube_resource_snapshot_response::fromCapnp(const curious::message::YoutubeResourceSnapshotResponse::Reader& reader) {
    youtube_resource_snapshot_response obj;

    obj._msgType = fromCapnpType(reader.getMsgType());
    obj._topic = reader.getTopic();
    obj._id = reader.getId();
    // Deserialize custom message field: request
    obj._request = request::fromCapnp(reader.getRequest());

    // Deserialize list field: resources
    auto ResourcesList = reader.getResources();
    obj._resources.clear();
    obj._resources.reserve(ResourcesList.size());
    for (const auto& item : ResourcesList) {
        obj._resources.push_back(youtube_resource::fromCapnp(item));
    }


    return obj;
}

std::string youtube_resource_snapshot_response::serialize() const {
    ::capnp::MallocMessageBuilder message;
    auto builder = message.initRoot<curious::message::YoutubeResourceSnapshotResponse>();
    toCapnp(builder);

    kj::VectorOutputStream output;
    capnp::writeMessage(output, message);
    auto data = output.getArray();
    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

youtube_resource_snapshot_response youtube_resource_snapshot_response::deserialize(const std::string& data) {
    try {
        kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
        ::capnp::InputStreamMessageReader message(input);
        auto reader = message.getRoot<curious::message::YoutubeResourceSnapshotResponse>();
        return fromCapnp(reader);
    } catch (const std::exception& e) {
        LOG_ERR << "Failed to deserialize YoutubeResourceSnapshotResponse: " << e.what() << go;
        return youtube_resource_snapshot_response{}; // Return default-constructed object
    }
}

}  // namespace curious::net
