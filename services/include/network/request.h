#pragma once
#include <messages/network_msg.capnp.h>
#include <string>
#include <cstdint>
#include <capnp/message.h>
#include <capnp/serialize.h>

#include <network/network_message.h>

#include <network/message_type.h>

namespace curious::net {

class request : public network_message {
  // add but dont delete this
  // #generated start
protected:
  int _id;
  std::string _reqGeneratedIp;
  std::string _reqGeneratedPort;

public:
  request() {
    // Initialize default values if needed
    // Initialize custom type message_type if needed
    _topic = "";
    _id = 0;
    _reqGeneratedIp = "";
    _reqGeneratedPort = "";
     _msgType = message_type::request;
  }

  message_type getMsgType() const { return _msgType; }
  void setMsgType(message_type value) { _msgType = value; }

  const std::string& getTopic() const { return _topic; }
  void setTopic(const std::string& value) { _topic = value; }

  int getId() const { return _id; }
  void setId(int value) { _id = value; }

  const std::string& getReqGeneratedIp() const { return _reqGeneratedIp; }
  void setReqGeneratedIp(const std::string& value) { _reqGeneratedIp = value; }

  const std::string& getReqGeneratedPort() const { return _reqGeneratedPort; }
  void setReqGeneratedPort(const std::string& value) { _reqGeneratedPort = value; }

  // Cap'n Proto conversion functions
  void toCapnp(curious::message::Request::Builder& builder) const;
  static request fromCapnp(const curious::message::Request::Reader& reader);

  // Serialization helpers
  std::string serialize() const;
  static request deserialize(const std::string& data);
  // #generated end
};

}  // namespace curious::net
