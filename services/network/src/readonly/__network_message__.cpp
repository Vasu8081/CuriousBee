#include <network/network_message.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>

namespace curious::net {

void network_message::toCapnp(curious::message::NetworkMessage::Builder& builder) const {
  builder.setMsgType(curious::message::MessageType::NETWORK_MESSAGE);
  builder.setTopic(_topic);
}

network_message network_message::fromCapnp(const curious::message::NetworkMessage::Reader& reader) {
  network_message obj;
  obj._msgType = fromCapnpType(reader.getMsgType());
  obj._topic = reader.getTopic();
  return obj;
}

std::string network_message::serialize() const {
  ::capnp::MallocMessageBuilder message;
  auto builder = message.initRoot<curious::message::NetworkMessage>();
  toCapnp(builder);
  
  kj::VectorOutputStream output;
  capnp::writeMessage(output, message);
  auto data = output.getArray();
  return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

network_message network_message::deserialize(const std::string& data) {
  kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
  ::capnp::InputStreamMessageReader message(input);
  auto reader = message.getRoot<curious::message::NetworkMessage>();
  return fromCapnp(reader);
}

}  // namespace curious::net
