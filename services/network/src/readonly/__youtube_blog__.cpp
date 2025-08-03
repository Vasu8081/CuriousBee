#include <network/youtube_blog.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>
#include <server/logger.h>

using namespace curious::net;
using namespace curious::log;

namespace curious::net {

void youtube_blog::toCapnp(curious::message::YoutubeBlog::Builder& builder) const {
    builder.setMsgType(curious::message::MessageType::YOUTUBE_BLOG);

    builder.setTopic(_topic);
    builder.setBlogId(_blogId);
    builder.setTitle(_title);
    builder.setSlug(_slug);
    builder.setCoverImageUrl(_coverImageUrl);
    builder.setPublishedDate(_publishedDate);
    builder.setContentHtml(_contentHtml);
}

youtube_blog youtube_blog::fromCapnp(const curious::message::YoutubeBlog::Reader& reader) {
    youtube_blog obj;

    obj._msgType = fromCapnpType(reader.getMsgType());
    obj._topic = reader.getTopic();
    obj._blogId = reader.getBlogId();
    obj._title = reader.getTitle();
    obj._slug = reader.getSlug();
    obj._coverImageUrl = reader.getCoverImageUrl();
    obj._publishedDate = reader.getPublishedDate();
    obj._contentHtml = reader.getContentHtml();

    return obj;
}

std::string youtube_blog::serialize() const {
    ::capnp::MallocMessageBuilder message;
    auto builder = message.initRoot<curious::message::YoutubeBlog>();
    toCapnp(builder);

    kj::VectorOutputStream output;
    capnp::writeMessage(output, message);
    auto data = output.getArray();
    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

youtube_blog youtube_blog::deserialize(const std::string& data) {
    try {
        kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
        ::capnp::InputStreamMessageReader message(input);
        auto reader = message.getRoot<curious::message::YoutubeBlog>();
        return fromCapnp(reader);
    } catch (const std::exception& e) {
        LOG_ERR << "Failed to deserialize YoutubeBlog: " << e.what() << go;
        return youtube_blog{}; // Return default-constructed object
    }
}

}  // namespace curious::net
