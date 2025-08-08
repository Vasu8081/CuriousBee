#include <network/youtube_blog_updates.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>
#include <server/logger.h>

using namespace curious::net;
using namespace curious::log;

namespace curious::net {

void youtube_blog_updates::toCapnp(curious::message::YoutubeBlogUpdates::Builder& builder) const {
    builder.setMsgType(curious::message::MessageType::YOUTUBE_BLOG_UPDATES);

    builder.setTopic(_topic);
    // Serialize list field: updates
    auto UpdatesList = builder.initUpdates(_updates.size());
    for (size_t i = 0; i < _updates.size(); ++i) {
        auto itemBuilder = UpdatesList[i];
        _updates[i].toCapnp(itemBuilder);
    }

}

youtube_blog_updates youtube_blog_updates::fromCapnp(const curious::message::YoutubeBlogUpdates::Reader& reader) {
    youtube_blog_updates obj;

    obj._msgType = fromCapnpType(reader.getMsgType());
    obj._topic = reader.getTopic();
    // Deserialize list field: updates
    auto UpdatesList = reader.getUpdates();
    obj._updates.clear();
    obj._updates.reserve(UpdatesList.size());
    for (const auto& item : UpdatesList) {
        obj._updates.push_back(youtube_blog::fromCapnp(item));
    }


    return obj;
}

std::string youtube_blog_updates::serialize() const {
    ::capnp::MallocMessageBuilder message;
    auto builder = message.initRoot<curious::message::YoutubeBlogUpdates>();
    toCapnp(builder);

    kj::VectorOutputStream output;
    capnp::writeMessage(output, message);
    auto data = output.getArray();
    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

youtube_blog_updates youtube_blog_updates::deserialize(const std::string& data) {
    try {
        kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
        ::capnp::InputStreamMessageReader message(input);
        auto reader = message.getRoot<curious::message::YoutubeBlogUpdates>();
        return fromCapnp(reader);
    } catch (const std::exception& e) {
        LOG_ERR << "Failed to deserialize YoutubeBlogUpdates: " << e.what() << go;
        return youtube_blog_updates{}; // Return default-constructed object
    }
}

}  // namespace curious::net
