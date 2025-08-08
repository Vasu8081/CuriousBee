#include <network/youtube_video_snapshot_response.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>
#include <server/logger.h>

using namespace curious::net;
using namespace curious::log;

namespace curious::net {

void youtube_video_snapshot_response::toCapnp(curious::message::YoutubeVideoSnapshotResponse::Builder& builder) const {
    builder.setMsgType(curious::message::MessageType::YOUTUBE_VIDEO_SNAPSHOT_RESPONSE);

    builder.setTopic(_topic);
    builder.setId(_id);
    // Serialize custom message field: request
    auto RequestBuilder = builder.initRequest();
    _request.toCapnp(RequestBuilder);

    // Serialize list field: videos
    auto VideosList = builder.initVideos(_videos.size());
    for (size_t i = 0; i < _videos.size(); ++i) {
        auto itemBuilder = VideosList[i];
        _videos[i].toCapnp(itemBuilder);
    }

}

youtube_video_snapshot_response youtube_video_snapshot_response::fromCapnp(const curious::message::YoutubeVideoSnapshotResponse::Reader& reader) {
    youtube_video_snapshot_response obj;

    obj._msgType = fromCapnpType(reader.getMsgType());
    obj._topic = reader.getTopic();
    obj._id = reader.getId();
    // Deserialize custom message field: request
    obj._request = request::fromCapnp(reader.getRequest());

    // Deserialize list field: videos
    auto VideosList = reader.getVideos();
    obj._videos.clear();
    obj._videos.reserve(VideosList.size());
    for (const auto& item : VideosList) {
        obj._videos.push_back(youtube_video::fromCapnp(item));
    }


    return obj;
}

std::string youtube_video_snapshot_response::serialize() const {
    ::capnp::MallocMessageBuilder message;
    auto builder = message.initRoot<curious::message::YoutubeVideoSnapshotResponse>();
    toCapnp(builder);

    kj::VectorOutputStream output;
    capnp::writeMessage(output, message);
    auto data = output.getArray();
    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

youtube_video_snapshot_response youtube_video_snapshot_response::deserialize(const std::string& data) {
    try {
        kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
        ::capnp::InputStreamMessageReader message(input);
        auto reader = message.getRoot<curious::message::YoutubeVideoSnapshotResponse>();
        return fromCapnp(reader);
    } catch (const std::exception& e) {
        LOG_ERR << "Failed to deserialize YoutubeVideoSnapshotResponse: " << e.what() << go;
        return youtube_video_snapshot_response{}; // Return default-constructed object
    }
}

}  // namespace curious::net
