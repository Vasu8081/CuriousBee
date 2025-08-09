#include <network/youtube_blog_snapshot_response.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>
#include <base/logger.h>

using namespace curious::net;
using namespace curious::log;

namespace curious::net {

void youtube_blog_snapshot_response::toCapnp(curious::message::YoutubeBlogSnapshotResponse::Builder& builder) const {
    builder.setMsgType(curious::message::MessageType::YOUTUBE_BLOG_SNAPSHOT_RESPONSE);

    builder.setTopic(_topic);
    builder.setId(_id);
    // Serialize custom message field: request
    auto RequestBuilder = builder.initRequest();
    _request.toCapnp(RequestBuilder);

    // Serialize list field: blogs
    auto BlogsList = builder.initBlogs(_blogs.size());
    for (size_t i = 0; i < _blogs.size(); ++i) {
        auto itemBuilder = BlogsList[i];
        _blogs[i].toCapnp(itemBuilder);
    }

}

youtube_blog_snapshot_response youtube_blog_snapshot_response::fromCapnp(const curious::message::YoutubeBlogSnapshotResponse::Reader& reader) {
    youtube_blog_snapshot_response obj;

    obj._msgType = fromCapnpType(reader.getMsgType());
    obj._topic = reader.getTopic();
    obj._id = reader.getId();
    // Deserialize custom message field: request
    obj._request = request::fromCapnp(reader.getRequest());

    // Deserialize list field: blogs
    auto BlogsList = reader.getBlogs();
    obj._blogs.clear();
    obj._blogs.reserve(BlogsList.size());
    for (const auto& item : BlogsList) {
        obj._blogs.push_back(youtube_blog::fromCapnp(item));
    }


    return obj;
}

std::string youtube_blog_snapshot_response::serialize() const {
    ::capnp::MallocMessageBuilder message;
    auto builder = message.initRoot<curious::message::YoutubeBlogSnapshotResponse>();
    toCapnp(builder);

    kj::VectorOutputStream output;
    capnp::writeMessage(output, message);
    auto data = output.getArray();
    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

youtube_blog_snapshot_response youtube_blog_snapshot_response::deserialize(const std::string& data) {
    try {
        kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
        ::capnp::InputStreamMessageReader message(input);
        auto reader = message.getRoot<curious::message::YoutubeBlogSnapshotResponse>();
        return fromCapnp(reader);
    } catch (const std::exception& e) {
        LOG_ERR << "Failed to deserialize YoutubeBlogSnapshotResponse: " << e.what() << go;
        return youtube_blog_snapshot_response{}; // Return default-constructed object
    }
}

}  // namespace curious::net
