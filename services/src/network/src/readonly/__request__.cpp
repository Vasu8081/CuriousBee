#include <network/request.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>
#include <base/logger.h>

using namespace curious::net;
using namespace curious::log;

namespace curious::net {

void request::toCapnp(curious::message::Request::Builder& builder) const {
    builder.setMsgType(curious::message::MessageType::REQUEST);

    builder.setTopic(_topic);
    builder.setId(_id);
    builder.setReqGeneratedIp(_reqGeneratedIp);
    builder.setReqGeneratedPort(_reqGeneratedPort);
}

request request::fromCapnp(const curious::message::Request::Reader& reader) {
    request obj;

    obj._msgType = fromCapnpType(reader.getMsgType());
    obj._topic = reader.getTopic();
    obj._id = reader.getId();
    obj._reqGeneratedIp = reader.getReqGeneratedIp();
    obj._reqGeneratedPort = reader.getReqGeneratedPort();

    return obj;
}

std::string request::serialize() const {
    ::capnp::MallocMessageBuilder message;
    auto builder = message.initRoot<curious::message::Request>();
    toCapnp(builder);

    kj::VectorOutputStream output;
    capnp::writeMessage(output, message);
    auto data = output.getArray();
    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

request request::deserialize(const std::string& data) {
    try {
        kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
        ::capnp::InputStreamMessageReader message(input);
        auto reader = message.getRoot<curious::message::Request>();
        return fromCapnp(reader);
    } catch (const std::exception& e) {
        LOG_ERR << "Failed to deserialize Request: " << e.what() << go;
        return request{}; // Return default-constructed object
    }
}

}  // namespace curious::net
