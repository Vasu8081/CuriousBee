#pragma once
#include <messages/network_msg.capnp.h>
#include <string>
#include <cstdint>
#include <capnp/message.h>
#include <capnp/serialize.h>

#include <network/message_type.h>

namespace curious::net {

class network_message {
public:
  bool is_request() const {
    return _msgType == message_type::request || _msgType == message_type::testRequest;
  }

  bool is_response() const {
    return _msgType == message_type::reply || _msgType == message_type::testReply;
  }

  bool is_network_message() const {
    return _msgType == message_type::networkMessage;
  }

  // #generated start
protected:
  message_type _msgType;
  std::string _topic;

public:
  network_message() {
    // Initialize default values if needed
    // Initialize custom type message_type if needed
    _topic = "";
     _msgType = message_type::networkMessage;
  }

  message_type getMsgType() const { return _msgType; }
  void setMsgType(message_type value) { _msgType = value; }

  const std::string& getTopic() const { return _topic; }
  void setTopic(const std::string& value) { _topic = value; }

  virtual ~network_message() = default;
  // Message type
  // Cap'n Proto conversion functions
  void toCapnp(curious::message::NetworkMessage::Builder& builder) const;
  static network_message fromCapnp(const curious::message::NetworkMessage::Reader& reader);

  // Serialization helpers
  std::string serialize() const;
  static network_message deserialize(const std::string& data);
  // #generated end
};

}  // namespace curious::net
