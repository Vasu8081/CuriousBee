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
  youtubeBlog = 17,
  youtubeBlogHeartbeat = 19,
  youtubeBlogSnapshotRequest = 16,
  youtubeBlogSnapshotResponse = 18,
  youtubeBlogUpdates = 20,
  youtubeResource = 12,
  youtubeResourceHeartbeat = 14,
  youtubeResourceSnapshotRequest = 11,
  youtubeResourceSnapshotResponse = 13,
  youtubeResourceUpdates = 15,
  youtubeVideo = 8,
  youtubeVideoHeartbeat = 9,
  youtubeVideoSnapshotRequest = 6,
  youtubeVideoSnapshotResponse = 7,
  youtubeVideoUpdates = 10,
};

inline std::string toString(message_type type) {
  switch (type) {
    case message_type::networkMessage: return "NetworkMessage";
    case message_type::reply: return "Reply";
    case message_type::request: return "Request";
    case message_type::testReply: return "TestReply";
    case message_type::testRequest: return "TestRequest";
    case message_type::youtubeBlog: return "YoutubeBlog";
    case message_type::youtubeBlogHeartbeat: return "YoutubeBlogHeartbeat";
    case message_type::youtubeBlogSnapshotRequest: return "YoutubeBlogSnapshotRequest";
    case message_type::youtubeBlogSnapshotResponse: return "YoutubeBlogSnapshotResponse";
    case message_type::youtubeBlogUpdates: return "YoutubeBlogUpdates";
    case message_type::youtubeResource: return "YoutubeResource";
    case message_type::youtubeResourceHeartbeat: return "YoutubeResourceHeartbeat";
    case message_type::youtubeResourceSnapshotRequest: return "YoutubeResourceSnapshotRequest";
    case message_type::youtubeResourceSnapshotResponse: return "YoutubeResourceSnapshotResponse";
    case message_type::youtubeResourceUpdates: return "YoutubeResourceUpdates";
    case message_type::youtubeVideo: return "YoutubeVideo";
    case message_type::youtubeVideoHeartbeat: return "YoutubeVideoHeartbeat";
    case message_type::youtubeVideoSnapshotRequest: return "YoutubeVideoSnapshotRequest";
    case message_type::youtubeVideoSnapshotResponse: return "YoutubeVideoSnapshotResponse";
    case message_type::youtubeVideoUpdates: return "YoutubeVideoUpdates";
    default: return "Unknown";
  }
}

inline message_type fromString(const std::string& str) {
  if (str == "NetworkMessage") return message_type::networkMessage;
  if (str == "Reply") return message_type::reply;
  if (str == "Request") return message_type::request;
  if (str == "TestReply") return message_type::testReply;
  if (str == "TestRequest") return message_type::testRequest;
  if (str == "YoutubeBlog") return message_type::youtubeBlog;
  if (str == "YoutubeBlogHeartbeat") return message_type::youtubeBlogHeartbeat;
  if (str == "YoutubeBlogSnapshotRequest") return message_type::youtubeBlogSnapshotRequest;
  if (str == "YoutubeBlogSnapshotResponse") return message_type::youtubeBlogSnapshotResponse;
  if (str == "YoutubeBlogUpdates") return message_type::youtubeBlogUpdates;
  if (str == "YoutubeResource") return message_type::youtubeResource;
  if (str == "YoutubeResourceHeartbeat") return message_type::youtubeResourceHeartbeat;
  if (str == "YoutubeResourceSnapshotRequest") return message_type::youtubeResourceSnapshotRequest;
  if (str == "YoutubeResourceSnapshotResponse") return message_type::youtubeResourceSnapshotResponse;
  if (str == "YoutubeResourceUpdates") return message_type::youtubeResourceUpdates;
  if (str == "YoutubeVideo") return message_type::youtubeVideo;
  if (str == "YoutubeVideoHeartbeat") return message_type::youtubeVideoHeartbeat;
  if (str == "YoutubeVideoSnapshotRequest") return message_type::youtubeVideoSnapshotRequest;
  if (str == "YoutubeVideoSnapshotResponse") return message_type::youtubeVideoSnapshotResponse;
  if (str == "YoutubeVideoUpdates") return message_type::youtubeVideoUpdates;
  return message_type::unknown;
}

inline message_type fromId(int id) {
  switch (id) {
    case 1: return message_type::networkMessage;
    case 3: return message_type::reply;
    case 2: return message_type::request;
    case 5: return message_type::testReply;
    case 4: return message_type::testRequest;
    case 17: return message_type::youtubeBlog;
    case 19: return message_type::youtubeBlogHeartbeat;
    case 16: return message_type::youtubeBlogSnapshotRequest;
    case 18: return message_type::youtubeBlogSnapshotResponse;
    case 20: return message_type::youtubeBlogUpdates;
    case 12: return message_type::youtubeResource;
    case 14: return message_type::youtubeResourceHeartbeat;
    case 11: return message_type::youtubeResourceSnapshotRequest;
    case 13: return message_type::youtubeResourceSnapshotResponse;
    case 15: return message_type::youtubeResourceUpdates;
    case 8: return message_type::youtubeVideo;
    case 9: return message_type::youtubeVideoHeartbeat;
    case 6: return message_type::youtubeVideoSnapshotRequest;
    case 7: return message_type::youtubeVideoSnapshotResponse;
    case 10: return message_type::youtubeVideoUpdates;
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
    case message_type::youtubeBlog: builder.setMsgType(curious::message::MessageType::YOUTUBE_BLOG); break;
    case message_type::youtubeBlogHeartbeat: builder.setMsgType(curious::message::MessageType::YOUTUBE_BLOG_HEARTBEAT); break;
    case message_type::youtubeBlogSnapshotRequest: builder.setMsgType(curious::message::MessageType::YOUTUBE_BLOG_SNAPSHOT_REQUEST); break;
    case message_type::youtubeBlogSnapshotResponse: builder.setMsgType(curious::message::MessageType::YOUTUBE_BLOG_SNAPSHOT_RESPONSE); break;
    case message_type::youtubeBlogUpdates: builder.setMsgType(curious::message::MessageType::YOUTUBE_BLOG_UPDATES); break;
    case message_type::youtubeResource: builder.setMsgType(curious::message::MessageType::YOUTUBE_RESOURCE); break;
    case message_type::youtubeResourceHeartbeat: builder.setMsgType(curious::message::MessageType::YOUTUBE_RESOURCE_HEARTBEAT); break;
    case message_type::youtubeResourceSnapshotRequest: builder.setMsgType(curious::message::MessageType::YOUTUBE_RESOURCE_SNAPSHOT_REQUEST); break;
    case message_type::youtubeResourceSnapshotResponse: builder.setMsgType(curious::message::MessageType::YOUTUBE_RESOURCE_SNAPSHOT_RESPONSE); break;
    case message_type::youtubeResourceUpdates: builder.setMsgType(curious::message::MessageType::YOUTUBE_RESOURCE_UPDATES); break;
    case message_type::youtubeVideo: builder.setMsgType(curious::message::MessageType::YOUTUBE_VIDEO); break;
    case message_type::youtubeVideoHeartbeat: builder.setMsgType(curious::message::MessageType::YOUTUBE_VIDEO_HEARTBEAT); break;
    case message_type::youtubeVideoSnapshotRequest: builder.setMsgType(curious::message::MessageType::YOUTUBE_VIDEO_SNAPSHOT_REQUEST); break;
    case message_type::youtubeVideoSnapshotResponse: builder.setMsgType(curious::message::MessageType::YOUTUBE_VIDEO_SNAPSHOT_RESPONSE); break;
    case message_type::youtubeVideoUpdates: builder.setMsgType(curious::message::MessageType::YOUTUBE_VIDEO_UPDATES); break;
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
    case curious::message::MessageType::YOUTUBE_BLOG: return message_type::youtubeBlog;
    case curious::message::MessageType::YOUTUBE_BLOG_HEARTBEAT: return message_type::youtubeBlogHeartbeat;
    case curious::message::MessageType::YOUTUBE_BLOG_SNAPSHOT_REQUEST: return message_type::youtubeBlogSnapshotRequest;
    case curious::message::MessageType::YOUTUBE_BLOG_SNAPSHOT_RESPONSE: return message_type::youtubeBlogSnapshotResponse;
    case curious::message::MessageType::YOUTUBE_BLOG_UPDATES: return message_type::youtubeBlogUpdates;
    case curious::message::MessageType::YOUTUBE_RESOURCE: return message_type::youtubeResource;
    case curious::message::MessageType::YOUTUBE_RESOURCE_HEARTBEAT: return message_type::youtubeResourceHeartbeat;
    case curious::message::MessageType::YOUTUBE_RESOURCE_SNAPSHOT_REQUEST: return message_type::youtubeResourceSnapshotRequest;
    case curious::message::MessageType::YOUTUBE_RESOURCE_SNAPSHOT_RESPONSE: return message_type::youtubeResourceSnapshotResponse;
    case curious::message::MessageType::YOUTUBE_RESOURCE_UPDATES: return message_type::youtubeResourceUpdates;
    case curious::message::MessageType::YOUTUBE_VIDEO: return message_type::youtubeVideo;
    case curious::message::MessageType::YOUTUBE_VIDEO_HEARTBEAT: return message_type::youtubeVideoHeartbeat;
    case curious::message::MessageType::YOUTUBE_VIDEO_SNAPSHOT_REQUEST: return message_type::youtubeVideoSnapshotRequest;
    case curious::message::MessageType::YOUTUBE_VIDEO_SNAPSHOT_RESPONSE: return message_type::youtubeVideoSnapshotResponse;
    case curious::message::MessageType::YOUTUBE_VIDEO_UPDATES: return message_type::youtubeVideoUpdates;
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
    case curious::message::MessageType::YOUTUBE_BLOG: return message_type::youtubeBlog;
    case curious::message::MessageType::YOUTUBE_BLOG_HEARTBEAT: return message_type::youtubeBlogHeartbeat;
    case curious::message::MessageType::YOUTUBE_BLOG_SNAPSHOT_REQUEST: return message_type::youtubeBlogSnapshotRequest;
    case curious::message::MessageType::YOUTUBE_BLOG_SNAPSHOT_RESPONSE: return message_type::youtubeBlogSnapshotResponse;
    case curious::message::MessageType::YOUTUBE_BLOG_UPDATES: return message_type::youtubeBlogUpdates;
    case curious::message::MessageType::YOUTUBE_RESOURCE: return message_type::youtubeResource;
    case curious::message::MessageType::YOUTUBE_RESOURCE_HEARTBEAT: return message_type::youtubeResourceHeartbeat;
    case curious::message::MessageType::YOUTUBE_RESOURCE_SNAPSHOT_REQUEST: return message_type::youtubeResourceSnapshotRequest;
    case curious::message::MessageType::YOUTUBE_RESOURCE_SNAPSHOT_RESPONSE: return message_type::youtubeResourceSnapshotResponse;
    case curious::message::MessageType::YOUTUBE_RESOURCE_UPDATES: return message_type::youtubeResourceUpdates;
    case curious::message::MessageType::YOUTUBE_VIDEO: return message_type::youtubeVideo;
    case curious::message::MessageType::YOUTUBE_VIDEO_HEARTBEAT: return message_type::youtubeVideoHeartbeat;
    case curious::message::MessageType::YOUTUBE_VIDEO_SNAPSHOT_REQUEST: return message_type::youtubeVideoSnapshotRequest;
    case curious::message::MessageType::YOUTUBE_VIDEO_SNAPSHOT_RESPONSE: return message_type::youtubeVideoSnapshotResponse;
    case curious::message::MessageType::YOUTUBE_VIDEO_UPDATES: return message_type::youtubeVideoUpdates;
    default: return message_type::unknown;
  }
}

}  // namespace curious::net
