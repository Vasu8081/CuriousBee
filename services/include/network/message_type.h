#pragma once
#include <string>
#include <cstdint>
#include <messages/network_msg.capnp.h>

namespace curious::net {

enum class message_type : int {
  unknown = 0,
  networkMessage = 1,
  reply = 3,
  request = 2,
  testReply = 5,
  testRequest = 4,
  youtubeVideo = 8,
  youtubeVideoSnapshotRequest = 6,
  youtubeVideoSnapshotResponse = 7,
};

inline std::string toString(message_type type) {
  switch (type) {
    case message_type::networkMessage: return "NetworkMessage";
    case message_type::reply: return "Reply";
    case message_type::request: return "Request";
    case message_type::testReply: return "TestReply";
    case message_type::testRequest: return "TestRequest";
    case message_type::youtubeVideo: return "YoutubeVideo";
    case message_type::youtubeVideoSnapshotRequest: return "YoutubeVideoSnapshotRequest";
    case message_type::youtubeVideoSnapshotResponse: return "YoutubeVideoSnapshotResponse";
    default: return "Unknown";
  }
}

inline message_type fromString(const std::string& str) {
  if (str == "NetworkMessage") return message_type::networkMessage;
  if (str == "Reply") return message_type::reply;
  if (str == "Request") return message_type::request;
  if (str == "TestReply") return message_type::testReply;
  if (str == "TestRequest") return message_type::testRequest;
  if (str == "YoutubeVideo") return message_type::youtubeVideo;
  if (str == "YoutubeVideoSnapshotRequest") return message_type::youtubeVideoSnapshotRequest;
  if (str == "YoutubeVideoSnapshotResponse") return message_type::youtubeVideoSnapshotResponse;
  return message_type::unknown;
}

inline message_type fromId(int id) {
  switch (id) {
    case 1: return message_type::networkMessage;
    case 3: return message_type::reply;
    case 2: return message_type::request;
    case 5: return message_type::testReply;
    case 4: return message_type::testRequest;
    case 8: return message_type::youtubeVideo;
    case 6: return message_type::youtubeVideoSnapshotRequest;
    case 7: return message_type::youtubeVideoSnapshotResponse;
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
    case message_type::youtubeVideo: builder.setMsgType(curious::message::MessageType::YOUTUBE_VIDEO); break;
    case message_type::youtubeVideoSnapshotRequest: builder.setMsgType(curious::message::MessageType::YOUTUBE_VIDEO_SNAPSHOT_REQUEST); break;
    case message_type::youtubeVideoSnapshotResponse: builder.setMsgType(curious::message::MessageType::YOUTUBE_VIDEO_SNAPSHOT_RESPONSE); break;
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
    case curious::message::MessageType::YOUTUBE_VIDEO: return message_type::youtubeVideo;
    case curious::message::MessageType::YOUTUBE_VIDEO_SNAPSHOT_REQUEST: return message_type::youtubeVideoSnapshotRequest;
    case curious::message::MessageType::YOUTUBE_VIDEO_SNAPSHOT_RESPONSE: return message_type::youtubeVideoSnapshotResponse;
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
    case curious::message::MessageType::YOUTUBE_VIDEO: return message_type::youtubeVideo;
    case curious::message::MessageType::YOUTUBE_VIDEO_SNAPSHOT_REQUEST: return message_type::youtubeVideoSnapshotRequest;
    case curious::message::MessageType::YOUTUBE_VIDEO_SNAPSHOT_RESPONSE: return message_type::youtubeVideoSnapshotResponse;
    default: return message_type::unknown;
  }
}

}  // namespace curious::net
