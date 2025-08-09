#include <network/youtube_blog_snapshot_request.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>
#include <base/logger.h>

using namespace curious::net;
using namespace curious::log;

namespace curious::net {

void youtube_blog_snapshot_request::toCapnp(curious::message::YoutubeBlogSnapshotRequest::Builder& builder) const {
    builder.setMsgType(curious::message::MessageType::YOUTUBE_BLOG_SNAPSHOT_REQUEST);

    builder.setTopic(_topic);
    builder.setId(_id);
    builder.setReqGeneratedIp(_reqGeneratedIp);
    builder.setReqGeneratedPort(_reqGeneratedPort);
}

youtube_blog_snapshot_request youtube_blog_snapshot_request::fromCapnp(const curious::message::YoutubeBlogSnapshotRequest::Reader& reader) {
    youtube_blog_snapshot_request obj;

    obj._msgType = fromCapnpType(reader.getMsgType());
    obj._topic = reader.getTopic();
    obj._id = reader.getId();
    obj._reqGeneratedIp = reader.getReqGeneratedIp();
    obj._reqGeneratedPort = reader.getReqGeneratedPort();

    return obj;
}

std::string youtube_blog_snapshot_request::serialize() const {
    ::capnp::MallocMessageBuilder message;
    auto builder = message.initRoot<curious::message::YoutubeBlogSnapshotRequest>();
    toCapnp(builder);

    kj::VectorOutputStream output;
    capnp::writeMessage(output, message);
    auto data = output.getArray();
    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

youtube_blog_snapshot_request youtube_blog_snapshot_request::deserialize(const std::string& data) {
    try {
        kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
        ::capnp::InputStreamMessageReader message(input);
        auto reader = message.getRoot<curious::message::YoutubeBlogSnapshotRequest>();
        return fromCapnp(reader);
    } catch (const std::exception& e) {
        LOG_ERR << "Failed to deserialize YoutubeBlogSnapshotRequest: " << e.what() << go;
        return youtube_blog_snapshot_request{}; // Return default-constructed object
    }
}

}  // namespace curious::net
