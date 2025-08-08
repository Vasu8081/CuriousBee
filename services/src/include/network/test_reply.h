#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <cstdint>
#include <network/reply.h>
#include <string>
//#editable_headers_start_dont_remove_this_line_only_write_below

//#editable_headers_end_dont_remove_this_line_only_write_above

namespace curious::net {

class test_reply : public reply {
protected:
  // Properties
  std::string _response;
  int _responseTest;
public:
  // Constructor
  test_reply() {
    _id = 0;
    _msgType = message_type::testReply;
    _response = "";
    _responseTest = 0;
    _topic = "";
  }

  std::string getResponse() const { return _response; }
  void setResponse(std::string value) { _response = value; }

  int getResponseTest() const { return _responseTest; }
  void setResponseTest(int value) { _responseTest = value; }

  void toCapnp(curious::message::TestReply::Builder& builder) const;
  static test_reply fromCapnp(const curious::message::TestReply::Reader& reader);
  std::string serialize() const;
  static test_reply deserialize(const std::string& data);
//#editable_class_start_dont_remove_this_line_only_write_above

//#editable_class_end_dont_remove_this_line_only_write_below
};
}  // namespace curious::net
