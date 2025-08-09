#include <network/youtube_video.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>
#include <base/logger.h>

using namespace curious::net;
using namespace curious::log;

namespace curious::net {

void youtube_video::toCapnp(curious::message::YoutubeVideo::Builder& builder) const {
    builder.setMsgType(curious::message::MessageType::YOUTUBE_VIDEO);

    builder.setTopic(_topic);
    builder.setVideoId(_videoId);
    builder.setTitle(_title);
    builder.setThumbnail(_thumbnail);
    builder.setThumbnailMedium(_thumbnailMedium);
    builder.setThumbnailHigh(_thumbnailHigh);
    builder.setThumbnailStandard(_thumbnailStandard);
    builder.setThumbnailMaxres(_thumbnailMaxres);
}

youtube_video youtube_video::fromCapnp(const curious::message::YoutubeVideo::Reader& reader) {
    youtube_video obj;

    obj._msgType = fromCapnpType(reader.getMsgType());
    obj._topic = reader.getTopic();
    obj._videoId = reader.getVideoId();
    obj._title = reader.getTitle();
    obj._thumbnail = reader.getThumbnail();
    obj._thumbnailMedium = reader.getThumbnailMedium();
    obj._thumbnailHigh = reader.getThumbnailHigh();
    obj._thumbnailStandard = reader.getThumbnailStandard();
    obj._thumbnailMaxres = reader.getThumbnailMaxres();

    return obj;
}

std::string youtube_video::serialize() const {
    ::capnp::MallocMessageBuilder message;
    auto builder = message.initRoot<curious::message::YoutubeVideo>();
    toCapnp(builder);

    kj::VectorOutputStream output;
    capnp::writeMessage(output, message);
    auto data = output.getArray();
    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

youtube_video youtube_video::deserialize(const std::string& data) {
    try {
        kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
        ::capnp::InputStreamMessageReader message(input);
        auto reader = message.getRoot<curious::message::YoutubeVideo>();
        return fromCapnp(reader);
    } catch (const std::exception& e) {
        LOG_ERR << "Failed to deserialize YoutubeVideo: " << e.what() << go;
        return youtube_video{}; // Return default-constructed object
    }
}

}  // namespace curious::net
