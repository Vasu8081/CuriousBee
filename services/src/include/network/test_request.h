#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <cstdint>
#include <network/request.h>
#include <string>
//#editable_headers_start_dont_remove_this_line_only_write_below

//#editable_headers_end_dont_remove_this_line_only_write_above

namespace curious::net {

class test_request : public request {
protected:
  // Properties
  std::string _message;
  std::string _user;
  int _age;
public:
  // Constructor
  test_request() {
    _age = 0;
    _id = 0;
    _message = "";
    _msgType = message_type::testRequest;
    _reqGeneratedIp = "";
    _reqGeneratedPort = "";
    _topic = "";
    _user = "";
  }

  std::string getMessage() const { return _message; }
  void setMessage(std::string value) { _message = value; }

  std::string getUser() const { return _user; }
  void setUser(std::string value) { _user = value; }

  int getAge() const { return _age; }
  void setAge(int value) { _age = value; }

  void toCapnp(curious::message::TestRequest::Builder& builder) const;
  static test_request fromCapnp(const curious::message::TestRequest::Reader& reader);
  std::string serialize() const;
  static test_request deserialize(const std::string& data);
//#editable_class_start_dont_remove_this_line_only_write_above

//#editable_class_end_dont_remove_this_line_only_write_below
};
}  // namespace curious::net
