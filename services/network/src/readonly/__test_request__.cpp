#include <network/test_request.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>

namespace curious::net {

void test_request::toCapnp(curious::message::TestRequest::Builder& builder) const {
  builder.setMsgType(curious::message::MessageType::TEST_REQUEST);
  builder.setId(_id);
  builder.setReqGeneratedIp(_reqGeneratedIp);
  builder.setReqGeneratedPort(_reqGeneratedPort);
  builder.setMessage(_message);
  builder.setUser(_user);
  builder.setAge(_age);
}

test_request test_request::fromCapnp(const curious::message::TestRequest::Reader& reader) {
  test_request obj;
  obj._id = reader.getId();
  obj._reqGeneratedIp = reader.getReqGeneratedIp();
  obj._reqGeneratedPort = reader.getReqGeneratedPort();
  obj._message = reader.getMessage();
  obj._user = reader.getUser();
  obj._age = reader.getAge();
  return obj;
}

std::string test_request::serialize() const {
  ::capnp::MallocMessageBuilder message;
  auto builder = message.initRoot<curious::message::TestRequest>();
  toCapnp(builder);
  
  kj::VectorOutputStream output;
  capnp::writeMessage(output, message);
  auto data = output.getArray();
  return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

test_request test_request::deserialize(const std::string& data) {
  kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
  ::capnp::InputStreamMessageReader message(input);
  auto reader = message.getRoot<curious::message::TestRequest>();
  return fromCapnp(reader);
}

}  // namespace curious::net
