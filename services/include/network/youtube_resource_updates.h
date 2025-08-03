#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <network/network_message.h>
#include <network/youtube_resource.h>
#include <vector>

namespace curious::net {

class youtube_resource_updates : public network_message {
protected:
  // Properties
  std::vector<youtube_resource> _updates;
public:
  // Constructor
  youtube_resource_updates() {
    _msgType = message_type::youtubeResourceUpdates;
    _topic = "";
    _updates = {};
  }

  std::vector<youtube_resource> getUpdates() const { return _updates; }
  void setUpdates(std::vector<youtube_resource> value) { _updates = value; }

  void toCapnp(curious::message::YoutubeResourceUpdates::Builder& builder) const;
  static youtube_resource_updates fromCapnp(const curious::message::YoutubeResourceUpdates::Reader& reader);
  std::string serialize() const;
  static youtube_resource_updates deserialize(const std::string& data);
};
}  // namespace curious::net
