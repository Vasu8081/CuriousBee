#pragma once
#include <messages/network_msg.capnp.h>
#include <string>
#include <cstdint>
#include <capnp/message.h>
#include <capnp/serialize.h>

#include <network/reply.h>

#include <network/request.h>

namespace curious::net {

class test_reply : public reply {
  // #generated start
protected:
  std::string _response;
  int _responseTest;

public:
  test_reply() {
    // Initialize default values if needed
    _id = 0;
    // Initialize custom type request if needed
    _response = "";
    _responseTest = 0;
     _msgType = message_type::testReply;
  }

  int getId() const { return _id; }
  void setId(int value) { _id = value; }

  request getRequest() const { return _request; }
  void setRequest(request value) { _request = value; }

  const std::string& getResponse() const { return _response; }
  void setResponse(const std::string& value) { _response = value; }

  int getResponseTest() const { return _responseTest; }
  void setResponseTest(int value) { _responseTest = value; }

  // Cap'n Proto conversion functions
  void toCapnp(curious::message::TestReply::Builder& builder) const;
  static test_reply fromCapnp(const curious::message::TestReply::Reader& reader);

  // Serialization helpers
  std::string serialize() const;
  static test_reply deserialize(const std::string& data);
  // #generated end
};

}  // namespace curious::net
