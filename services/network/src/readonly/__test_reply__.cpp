#include <network/test_reply.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>

namespace curious::net {

void test_reply::toCapnp(curious::message::TestReply::Builder& builder) const {
  builder.setMsgType(curious::message::MessageType::TEST_REPLY);
  builder.setId(_id);
  auto RequestBuilder = builder.initRequest();
  _request.toCapnp(RequestBuilder);
  builder.setResponse(_response);
  builder.setResponseTest(_responseTest);
}

test_reply test_reply::fromCapnp(const curious::message::TestReply::Reader& reader) {
  test_reply obj;
  obj._id = reader.getId();
  obj._request = request::fromCapnp(reader.getRequest());
  obj._response = reader.getResponse();
  obj._responseTest = reader.getResponseTest();
  return obj;
}

std::string test_reply::serialize() const {
  ::capnp::MallocMessageBuilder message;
  auto builder = message.initRoot<curious::message::TestReply>();
  toCapnp(builder);
  
  kj::VectorOutputStream output;
  capnp::writeMessage(output, message);
  auto data = output.getArray();
  return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

test_reply test_reply::deserialize(const std::string& data) {
  kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
  ::capnp::InputStreamMessageReader message(input);
  auto reader = message.getRoot<curious::message::TestReply>();
  return fromCapnp(reader);
}

}  // namespace curious::net
