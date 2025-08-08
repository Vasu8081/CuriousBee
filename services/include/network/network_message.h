#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <network/message_type.h>
#include <string>
//#editable_headers_start_dont_remove_this_line_only_write_below

//#editable_headers_end_dont_remove_this_line_only_write_above

namespace curious::net {

class network_message {
protected:
  // Properties
  message_type _msgType;
  std::string _topic;
public:
  // Constructor
  network_message() {
    _msgType = message_type::networkMessage;
    _topic = "";
  }

  message_type getMsgType() const { return _msgType; }
  void setMsgType(message_type value) { _msgType = value; }

  std::string getTopic() const { return _topic; }
  void setTopic(std::string value) { _topic = value; }

  void toCapnp(curious::message::NetworkMessage::Builder& builder) const;
  static network_message fromCapnp(const curious::message::NetworkMessage::Reader& reader);
  std::string serialize() const;
  static network_message deserialize(const std::string& data);
//#editable_class_start_dont_remove_this_line_only_write_above

//#editable_class_end_dont_remove_this_line_only_write_below
};
}  // namespace curious::net
