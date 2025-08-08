#include <network/reply.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>
#include <server/logger.h>

using namespace curious::net;
using namespace curious::log;

namespace curious::net {

void reply::toCapnp(curious::message::Reply::Builder& builder) const {
    builder.setMsgType(curious::message::MessageType::REPLY);

    builder.setTopic(_topic);
    builder.setId(_id);
    // Serialize custom message field: request
    auto RequestBuilder = builder.initRequest();
    _request.toCapnp(RequestBuilder);

}

reply reply::fromCapnp(const curious::message::Reply::Reader& reader) {
    reply obj;

    obj._msgType = fromCapnpType(reader.getMsgType());
    obj._topic = reader.getTopic();
    obj._id = reader.getId();
    // Deserialize custom message field: request
    obj._request = request::fromCapnp(reader.getRequest());


    return obj;
}

std::string reply::serialize() const {
    ::capnp::MallocMessageBuilder message;
    auto builder = message.initRoot<curious::message::Reply>();
    toCapnp(builder);

    kj::VectorOutputStream output;
    capnp::writeMessage(output, message);
    auto data = output.getArray();
    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

reply reply::deserialize(const std::string& data) {
    try {
        kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
        ::capnp::InputStreamMessageReader message(input);
        auto reader = message.getRoot<curious::message::Reply>();
        return fromCapnp(reader);
    } catch (const std::exception& e) {
        LOG_ERR << "Failed to deserialize Reply: " << e.what() << go;
        return reply{}; // Return default-constructed object
    }
}

}  // namespace curious::net
