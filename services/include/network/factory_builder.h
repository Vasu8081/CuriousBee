#pragma once
#include <network/message_type.h>
#include <memory>
#include <string>

#include <network/network_message.h>
#include <network/reply.h>
#include <network/request.h>
#include <network/test_reply.h>
#include <network/test_request.h>
namespace curious::net {

class FactoryBuilder {
public:
  static std::shared_ptr<network_message> createMessage(message_type type) {
    switch (type) {
      case message_type::networkMessage: return std::make_shared<network_message>();
      case message_type::reply: return std::make_shared<reply>();
      case message_type::request: return std::make_shared<request>();
      case message_type::testReply: return std::make_shared<test_reply>();
      case message_type::testRequest: return std::make_shared<test_request>();
      default: return nullptr; // Unknown message type
    }
  }

  static std::shared_ptr<network_message> createMessage(const std::string &typeName) {
    message_type type = fromString(typeName);
    return createMessage(type);
  }

  static std::shared_ptr<network_message> createMessage(int id) {
    message_type type = fromId(id);
    return createMessage(type);
  }

  static std::shared_ptr<network_message> fromCapnp(capnp::MessageReader &reader) {
    auto msgType = curious::net::fromCapnp(reader.getRoot<curious::message::NetworkMessage>());
    switch (msgType) {
      case message_type::networkMessage: {
        auto typedMsg = std::make_shared<network_message>(network_message::fromCapnp(reader.getRoot<curious::message::NetworkMessage>()));
        return typedMsg;
      }
      case message_type::reply: {
        auto typedMsg = std::make_shared<reply>(reply::fromCapnp(reader.getRoot<curious::message::Reply>()));
        return typedMsg;
      }
      case message_type::request: {
        auto typedMsg = std::make_shared<request>(request::fromCapnp(reader.getRoot<curious::message::Request>()));
        return typedMsg;
      }
      case message_type::testReply: {
        auto typedMsg = std::make_shared<test_reply>(test_reply::fromCapnp(reader.getRoot<curious::message::TestReply>()));
        return typedMsg;
      }
      case message_type::testRequest: {
        auto typedMsg = std::make_shared<test_request>(test_request::fromCapnp(reader.getRoot<curious::message::TestRequest>()));
        return typedMsg;
      }
      default: return nullptr; // Unknown message type
    }
  }

  static void toCapnp(capnp::MallocMessageBuilder& builder, const std::shared_ptr<network_message>& msg) {
    switch (msg->getMsgType()) {
      case message_type::networkMessage: {
        auto root = builder.initRoot<curious::message::NetworkMessage>();
        auto typedMsg = std::dynamic_pointer_cast<network_message>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type network_message");
        }
        break;
      }
      case message_type::reply: {
        auto root = builder.initRoot<curious::message::Reply>();
        auto typedMsg = std::dynamic_pointer_cast<reply>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type reply");
        }
        break;
      }
      case message_type::request: {
        auto root = builder.initRoot<curious::message::Request>();
        auto typedMsg = std::dynamic_pointer_cast<request>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type request");
        }
        break;
      }
      case message_type::testReply: {
        auto root = builder.initRoot<curious::message::TestReply>();
        auto typedMsg = std::dynamic_pointer_cast<test_reply>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type test_reply");
        }
        break;
      }
      case message_type::testRequest: {
        auto root = builder.initRoot<curious::message::TestRequest>();
        auto typedMsg = std::dynamic_pointer_cast<test_request>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type test_request");
        }
        break;
      }
      default: throw std::runtime_error("Unknown message type");
    }
  }

};

}  // namespace curious::net
