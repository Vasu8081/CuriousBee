#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <cstdint>
#include <network/network_message.h>
#include <network/request.h>
//#editable_headers_start_dont_remove_this_line_only_write_below

//#editable_headers_end_dont_remove_this_line_only_write_above

namespace curious::net {

class reply : public network_message {
protected:
  // Properties
  int _id;
  request _request;
public:
  // Constructor
  reply() {
    _id = 0;
    _msgType = message_type::reply;
    _topic = "";
  }

  int getId() const { return _id; }
  void setId(int value) { _id = value; }

  request getRequest() const { return _request; }
  void setRequest(request value) { _request = value; }

  void toCapnp(curious::message::Reply::Builder& builder) const;
  static reply fromCapnp(const curious::message::Reply::Reader& reader);
  std::string serialize() const;
  static reply deserialize(const std::string& data);
//#editable_class_start_dont_remove_this_line_only_write_above

//#editable_class_end_dont_remove_this_line_only_write_below
};
}  // namespace curious::net
