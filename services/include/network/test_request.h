#pragma once
#include <messages/network_msg.capnp.h>
#include <string>
#include <cstdint>
#include <capnp/message.h>
#include <capnp/serialize.h>

#include <network/request.h>


namespace curious::net {

class test_request : public request {
  // #generated start
protected:
  std::string _message;
  std::string _user;
  int _age;

public:
  test_request() {
    // Initialize default values if needed
    _id = 0;
    _reqGeneratedIp = "";
    _reqGeneratedPort = "";
    _message = "";
    _user = "";
    _age = 0;
     _msgType = message_type::testRequest;
  }

  int getId() const { return _id; }
  void setId(int value) { _id = value; }

  const std::string& getReqGeneratedIp() const { return _reqGeneratedIp; }
  void setReqGeneratedIp(const std::string& value) { _reqGeneratedIp = value; }

  const std::string& getReqGeneratedPort() const { return _reqGeneratedPort; }
  void setReqGeneratedPort(const std::string& value) { _reqGeneratedPort = value; }

  const std::string& getMessage() const { return _message; }
  void setMessage(const std::string& value) { _message = value; }

  const std::string& getUser() const { return _user; }
  void setUser(const std::string& value) { _user = value; }

  int getAge() const { return _age; }
  void setAge(int value) { _age = value; }

  // Cap'n Proto conversion functions
  void toCapnp(curious::message::TestRequest::Builder& builder) const;
  static test_request fromCapnp(const curious::message::TestRequest::Reader& reader);

  // Serialization helpers
  std::string serialize() const;
  static test_request deserialize(const std::string& data);
  // #generated end
};

}  // namespace curious::net
