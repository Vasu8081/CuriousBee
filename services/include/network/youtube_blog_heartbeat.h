#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <cstdint>
#include <network/network_message.h>

namespace curious::net {

class youtube_blog_heartbeat : public network_message {
protected:
  // Properties
  int _resourcesCount;
public:
  // Constructor
  youtube_blog_heartbeat() {
    _msgType = message_type::youtubeBlogHeartbeat;
    _resourcesCount = 0;
    _topic = "";
  }

  int getResourcesCount() const { return _resourcesCount; }
  void setResourcesCount(int value) { _resourcesCount = value; }

  void toCapnp(curious::message::YoutubeBlogHeartbeat::Builder& builder) const;
  static youtube_blog_heartbeat fromCapnp(const curious::message::YoutubeBlogHeartbeat::Reader& reader);
  std::string serialize() const;
  static youtube_blog_heartbeat deserialize(const std::string& data);
};
}  // namespace curious::net
