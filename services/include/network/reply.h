#pragma once
#include <messages/network_msg.capnp.h>
#include <string>
#include <cstdint>
#include <capnp/message.h>
#include <capnp/serialize.h>

#include <network/network_message.h>

#include <network/message_type.h>
#include <network/request.h>

namespace curious::net {

class reply : public network_message {
  // #generated start
protected:
  int _id;
  request _request;

public:
  reply() {
    // Initialize default values if needed
    // Initialize custom type message_type if needed
    _topic = "";
    _id = 0;
    // Initialize custom type request if needed
     _msgType = message_type::reply;
  }

  message_type getMsgType() const { return _msgType; }
  void setMsgType(message_type value) { _msgType = value; }

  const std::string& getTopic() const { return _topic; }
  void setTopic(const std::string& value) { _topic = value; }

  int getId() const { return _id; }
  void setId(int value) { _id = value; }

  request getRequest() const { return _request; }
  void setRequest(request value) { _request = value; }

  // Cap'n Proto conversion functions
  void toCapnp(curious::message::Reply::Builder& builder) const;
  static reply fromCapnp(const curious::message::Reply::Reader& reader);

  // Serialization helpers
  std::string serialize() const;
  static reply deserialize(const std::string& data);
  // #generated end
};

}  // namespace curious::net
