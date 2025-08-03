#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <cstdint>
#include <network/network_message.h>
#include <string>
//#editable_headers_start_dont_remove_this_line_only_write_below

//#editable_headers_end_dont_remove_this_line_only_write_above

namespace curious::net {

class request : public network_message {
protected:
  // Properties
  int _id;
  std::string _reqGeneratedIp;
  std::string _reqGeneratedPort;
public:
  // Constructor
  request() {
    _id = 0;
    _msgType = message_type::request;
    _reqGeneratedIp = "";
    _reqGeneratedPort = "";
    _topic = "";
  }

  int getId() const { return _id; }
  void setId(int value) { _id = value; }

  std::string getReqGeneratedIp() const { return _reqGeneratedIp; }
  void setReqGeneratedIp(std::string value) { _reqGeneratedIp = value; }

  std::string getReqGeneratedPort() const { return _reqGeneratedPort; }
  void setReqGeneratedPort(std::string value) { _reqGeneratedPort = value; }

  void toCapnp(curious::message::Request::Builder& builder) const;
  static request fromCapnp(const curious::message::Request::Reader& reader);
  std::string serialize() const;
  static request deserialize(const std::string& data);
//#editable_class_start_dont_remove_this_line_only_write_above

//#editable_class_end_dont_remove_this_line_only_write_below
};
}  // namespace curious::net
