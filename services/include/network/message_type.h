#pragma once
#include <string>
#include <cstdint>

namespace curious::net {

enum class message_type : int {
  unknown = 0,
  networkMessage = 1,
  reply = 3,
  request = 2,
  testReply = 5,
  testRequest = 4,
};

inline std::string toString(message_type type) {
  switch (type) {
    case message_type::networkMessage: return "NetworkMessage";
    case message_type::reply: return "Reply";
    case message_type::request: return "Request";
    case message_type::testReply: return "TestReply";
    case message_type::testRequest: return "TestRequest";
    default: return "Unknown";
  }
}

inline message_type fromString(const std::string& str) {
  if (str == "NetworkMessage") return message_type::networkMessage;
  if (str == "Reply") return message_type::reply;
  if (str == "Request") return message_type::request;
  if (str == "TestReply") return message_type::testReply;
  if (str == "TestRequest") return message_type::testRequest;
  return message_type::unknown;
}

inline message_type fromId(int id) {
  switch (id) {
    case 1: return message_type::networkMessage;
    case 3: return message_type::reply;
    case 2: return message_type::request;
    case 5: return message_type::testReply;
    case 4: return message_type::testRequest;
    default: return message_type::unknown;
  }
}

inline void toCapnp(message_type type, curious::message::NetworkMessage::Builder& builder) {
  switch (type) {
    case message_type::networkMessage: builder.setMsgType(curious::message::MessageType::NETWORK_MESSAGE); break;
    case message_type::reply: builder.setMsgType(curious::message::MessageType::REPLY); break;
    case message_type::request: builder.setMsgType(curious::message::MessageType::REQUEST); break;
    case message_type::testReply: builder.setMsgType(curious::message::MessageType::TEST_REPLY); break;
    case message_type::testRequest: builder.setMsgType(curious::message::MessageType::TEST_REQUEST); break;
    default: builder.setMsgType(curious::message::MessageType::UNKNOWN); break;
  }
}

inline message_type fromCapnp(const curious::message::NetworkMessage::Reader& reader) {
  switch (reader.getMsgType()) {
    case curious::message::MessageType::NETWORK_MESSAGE: return message_type::networkMessage;
    case curious::message::MessageType::REPLY: return message_type::reply;
    case curious::message::MessageType::REQUEST: return message_type::request;
    case curious::message::MessageType::TEST_REPLY: return message_type::testReply;
    case curious::message::MessageType::TEST_REQUEST: return message_type::testRequest;
    default: return message_type::unknown;
  }
}

inline message_type fromCapnpType(curious::message::MessageType type) {
  switch (type) {
    case curious::message::MessageType::NETWORK_MESSAGE: return message_type::networkMessage;
    case curious::message::MessageType::REPLY: return message_type::reply;
    case curious::message::MessageType::REQUEST: return message_type::request;
    case curious::message::MessageType::TEST_REPLY: return message_type::testReply;
    case curious::message::MessageType::TEST_REQUEST: return message_type::testRequest;
    default: return message_type::unknown;
  }
}

}  // namespace curious::net
