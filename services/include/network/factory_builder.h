#pragma once
#include <network/message_type.h>
#include <memory>
#include <string>
#include <stdexcept>
#include <capnp/message.h>

#include <network/network_message.h>
#include <network/reply.h>
#include <network/request.h>
#include <network/test_reply.h>
#include <network/test_request.h>
#include <network/youtube_blog.h>
#include <network/youtube_blog_heartbeat.h>
#include <network/youtube_blog_snapshot_request.h>
#include <network/youtube_blog_snapshot_response.h>
#include <network/youtube_blog_updates.h>
#include <network/youtube_resource.h>
#include <network/youtube_resource_heartbeat.h>
#include <network/youtube_resource_snapshot_request.h>
#include <network/youtube_resource_snapshot_response.h>
#include <network/youtube_resource_updates.h>
#include <network/youtube_video.h>
#include <network/youtube_video_heartbeat.h>
#include <network/youtube_video_snapshot_request.h>
#include <network/youtube_video_snapshot_response.h>
#include <network/youtube_video_updates.h>

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
      case message_type::youtubeBlog: return std::make_shared<youtube_blog>();
      case message_type::youtubeBlogHeartbeat: return std::make_shared<youtube_blog_heartbeat>();
      case message_type::youtubeBlogSnapshotRequest: return std::make_shared<youtube_blog_snapshot_request>();
      case message_type::youtubeBlogSnapshotResponse: return std::make_shared<youtube_blog_snapshot_response>();
      case message_type::youtubeBlogUpdates: return std::make_shared<youtube_blog_updates>();
      case message_type::youtubeResource: return std::make_shared<youtube_resource>();
      case message_type::youtubeResourceHeartbeat: return std::make_shared<youtube_resource_heartbeat>();
      case message_type::youtubeResourceSnapshotRequest: return std::make_shared<youtube_resource_snapshot_request>();
      case message_type::youtubeResourceSnapshotResponse: return std::make_shared<youtube_resource_snapshot_response>();
      case message_type::youtubeResourceUpdates: return std::make_shared<youtube_resource_updates>();
      case message_type::youtubeVideo: return std::make_shared<youtube_video>();
      case message_type::youtubeVideoHeartbeat: return std::make_shared<youtube_video_heartbeat>();
      case message_type::youtubeVideoSnapshotRequest: return std::make_shared<youtube_video_snapshot_request>();
      case message_type::youtubeVideoSnapshotResponse: return std::make_shared<youtube_video_snapshot_response>();
      case message_type::youtubeVideoUpdates: return std::make_shared<youtube_video_updates>();
      default: return nullptr; // Unknown message type
    }
  }

  static std::shared_ptr<network_message> createMessage(const std::string& typeName) {
    message_type type = fromString(typeName);
    return createMessage(type);
  }

  static std::shared_ptr<network_message> createMessage(int id) {
    message_type type = fromId(id);
    return createMessage(type);
  }

  static std::shared_ptr<network_message> fromCapnp(capnp::MessageReader& reader) {
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
      case message_type::youtubeBlog: {
        auto typedMsg = std::make_shared<youtube_blog>(youtube_blog::fromCapnp(reader.getRoot<curious::message::YoutubeBlog>()));
        return typedMsg;
      }
      case message_type::youtubeBlogHeartbeat: {
        auto typedMsg = std::make_shared<youtube_blog_heartbeat>(youtube_blog_heartbeat::fromCapnp(reader.getRoot<curious::message::YoutubeBlogHeartbeat>()));
        return typedMsg;
      }
      case message_type::youtubeBlogSnapshotRequest: {
        auto typedMsg = std::make_shared<youtube_blog_snapshot_request>(youtube_blog_snapshot_request::fromCapnp(reader.getRoot<curious::message::YoutubeBlogSnapshotRequest>()));
        return typedMsg;
      }
      case message_type::youtubeBlogSnapshotResponse: {
        auto typedMsg = std::make_shared<youtube_blog_snapshot_response>(youtube_blog_snapshot_response::fromCapnp(reader.getRoot<curious::message::YoutubeBlogSnapshotResponse>()));
        return typedMsg;
      }
      case message_type::youtubeBlogUpdates: {
        auto typedMsg = std::make_shared<youtube_blog_updates>(youtube_blog_updates::fromCapnp(reader.getRoot<curious::message::YoutubeBlogUpdates>()));
        return typedMsg;
      }
      case message_type::youtubeResource: {
        auto typedMsg = std::make_shared<youtube_resource>(youtube_resource::fromCapnp(reader.getRoot<curious::message::YoutubeResource>()));
        return typedMsg;
      }
      case message_type::youtubeResourceHeartbeat: {
        auto typedMsg = std::make_shared<youtube_resource_heartbeat>(youtube_resource_heartbeat::fromCapnp(reader.getRoot<curious::message::YoutubeResourceHeartbeat>()));
        return typedMsg;
      }
      case message_type::youtubeResourceSnapshotRequest: {
        auto typedMsg = std::make_shared<youtube_resource_snapshot_request>(youtube_resource_snapshot_request::fromCapnp(reader.getRoot<curious::message::YoutubeResourceSnapshotRequest>()));
        return typedMsg;
      }
      case message_type::youtubeResourceSnapshotResponse: {
        auto typedMsg = std::make_shared<youtube_resource_snapshot_response>(youtube_resource_snapshot_response::fromCapnp(reader.getRoot<curious::message::YoutubeResourceSnapshotResponse>()));
        return typedMsg;
      }
      case message_type::youtubeResourceUpdates: {
        auto typedMsg = std::make_shared<youtube_resource_updates>(youtube_resource_updates::fromCapnp(reader.getRoot<curious::message::YoutubeResourceUpdates>()));
        return typedMsg;
      }
      case message_type::youtubeVideo: {
        auto typedMsg = std::make_shared<youtube_video>(youtube_video::fromCapnp(reader.getRoot<curious::message::YoutubeVideo>()));
        return typedMsg;
      }
      case message_type::youtubeVideoHeartbeat: {
        auto typedMsg = std::make_shared<youtube_video_heartbeat>(youtube_video_heartbeat::fromCapnp(reader.getRoot<curious::message::YoutubeVideoHeartbeat>()));
        return typedMsg;
      }
      case message_type::youtubeVideoSnapshotRequest: {
        auto typedMsg = std::make_shared<youtube_video_snapshot_request>(youtube_video_snapshot_request::fromCapnp(reader.getRoot<curious::message::YoutubeVideoSnapshotRequest>()));
        return typedMsg;
      }
      case message_type::youtubeVideoSnapshotResponse: {
        auto typedMsg = std::make_shared<youtube_video_snapshot_response>(youtube_video_snapshot_response::fromCapnp(reader.getRoot<curious::message::YoutubeVideoSnapshotResponse>()));
        return typedMsg;
      }
      case message_type::youtubeVideoUpdates: {
        auto typedMsg = std::make_shared<youtube_video_updates>(youtube_video_updates::fromCapnp(reader.getRoot<curious::message::YoutubeVideoUpdates>()));
        return typedMsg;
      }
      default: return nullptr; // Unknown message type
    }
  }

  static void toCapnp(capnp::MallocMessageBuilder& builder, const std::shared_ptr<network_message>& msg) {
    if (!msg) {
      throw std::runtime_error("Cannot serialize null message");
    }

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
      case message_type::youtubeBlog: {
        auto root = builder.initRoot<curious::message::YoutubeBlog>();
        auto typedMsg = std::dynamic_pointer_cast<youtube_blog>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type youtube_blog");
        }
        break;
      }
      case message_type::youtubeBlogHeartbeat: {
        auto root = builder.initRoot<curious::message::YoutubeBlogHeartbeat>();
        auto typedMsg = std::dynamic_pointer_cast<youtube_blog_heartbeat>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type youtube_blog_heartbeat");
        }
        break;
      }
      case message_type::youtubeBlogSnapshotRequest: {
        auto root = builder.initRoot<curious::message::YoutubeBlogSnapshotRequest>();
        auto typedMsg = std::dynamic_pointer_cast<youtube_blog_snapshot_request>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type youtube_blog_snapshot_request");
        }
        break;
      }
      case message_type::youtubeBlogSnapshotResponse: {
        auto root = builder.initRoot<curious::message::YoutubeBlogSnapshotResponse>();
        auto typedMsg = std::dynamic_pointer_cast<youtube_blog_snapshot_response>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type youtube_blog_snapshot_response");
        }
        break;
      }
      case message_type::youtubeBlogUpdates: {
        auto root = builder.initRoot<curious::message::YoutubeBlogUpdates>();
        auto typedMsg = std::dynamic_pointer_cast<youtube_blog_updates>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type youtube_blog_updates");
        }
        break;
      }
      case message_type::youtubeResource: {
        auto root = builder.initRoot<curious::message::YoutubeResource>();
        auto typedMsg = std::dynamic_pointer_cast<youtube_resource>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type youtube_resource");
        }
        break;
      }
      case message_type::youtubeResourceHeartbeat: {
        auto root = builder.initRoot<curious::message::YoutubeResourceHeartbeat>();
        auto typedMsg = std::dynamic_pointer_cast<youtube_resource_heartbeat>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type youtube_resource_heartbeat");
        }
        break;
      }
      case message_type::youtubeResourceSnapshotRequest: {
        auto root = builder.initRoot<curious::message::YoutubeResourceSnapshotRequest>();
        auto typedMsg = std::dynamic_pointer_cast<youtube_resource_snapshot_request>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type youtube_resource_snapshot_request");
        }
        break;
      }
      case message_type::youtubeResourceSnapshotResponse: {
        auto root = builder.initRoot<curious::message::YoutubeResourceSnapshotResponse>();
        auto typedMsg = std::dynamic_pointer_cast<youtube_resource_snapshot_response>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type youtube_resource_snapshot_response");
        }
        break;
      }
      case message_type::youtubeResourceUpdates: {
        auto root = builder.initRoot<curious::message::YoutubeResourceUpdates>();
        auto typedMsg = std::dynamic_pointer_cast<youtube_resource_updates>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type youtube_resource_updates");
        }
        break;
      }
      case message_type::youtubeVideo: {
        auto root = builder.initRoot<curious::message::YoutubeVideo>();
        auto typedMsg = std::dynamic_pointer_cast<youtube_video>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type youtube_video");
        }
        break;
      }
      case message_type::youtubeVideoHeartbeat: {
        auto root = builder.initRoot<curious::message::YoutubeVideoHeartbeat>();
        auto typedMsg = std::dynamic_pointer_cast<youtube_video_heartbeat>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type youtube_video_heartbeat");
        }
        break;
      }
      case message_type::youtubeVideoSnapshotRequest: {
        auto root = builder.initRoot<curious::message::YoutubeVideoSnapshotRequest>();
        auto typedMsg = std::dynamic_pointer_cast<youtube_video_snapshot_request>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type youtube_video_snapshot_request");
        }
        break;
      }
      case message_type::youtubeVideoSnapshotResponse: {
        auto root = builder.initRoot<curious::message::YoutubeVideoSnapshotResponse>();
        auto typedMsg = std::dynamic_pointer_cast<youtube_video_snapshot_response>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type youtube_video_snapshot_response");
        }
        break;
      }
      case message_type::youtubeVideoUpdates: {
        auto root = builder.initRoot<curious::message::YoutubeVideoUpdates>();
        auto typedMsg = std::dynamic_pointer_cast<youtube_video_updates>(msg);
        if (typedMsg) {
          typedMsg->toCapnp(root);
        } else {
          throw std::runtime_error("Failed to cast message to type youtube_video_updates");
        }
        break;
      }
      default: throw std::runtime_error("Unknown message type");
    }
  }

  static std::string serialize(const std::shared_ptr<network_message>& msg) {
    if (!msg) {
      throw std::runtime_error("Cannot serialize null message");
    }

    capnp::MallocMessageBuilder builder;
    toCapnp(builder, msg);

    kj::VectorOutputStream output;
    capnp::writeMessage(output, builder);
    auto data = output.getArray();
    return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
  }

  static std::shared_ptr<network_message> deserialize(const std::string& data) {
    if (data.empty()) {
      throw std::runtime_error("Cannot deserialize empty data");
    }

    try {
      kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
      capnp::InputStreamMessageReader reader(input);
      return fromCapnp(reader);
    } catch (const std::exception& e) {
      throw std::runtime_error(std::string("Failed to deserialize message: ") + e.what());
    }
  }

};

}  // namespace curious::net
