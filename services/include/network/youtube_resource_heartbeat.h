#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <cstdint>
#include <network/network_message.h>
//#editable_headers_start_dont_remove_this_line_only_write_below

//#editable_headers_end_dont_remove_this_line_only_write_above

namespace curious::net {

class youtube_resource_heartbeat : public network_message {
protected:
  // Properties
  int _resourcesCount;
public:
  // Constructor
  youtube_resource_heartbeat() {
    _msgType = message_type::youtubeResourceHeartbeat;
    _resourcesCount = 0;
    _topic = "";
  }

  int getResourcesCount() const { return _resourcesCount; }
  void setResourcesCount(int value) { _resourcesCount = value; }

  void toCapnp(curious::message::YoutubeResourceHeartbeat::Builder& builder) const;
  static youtube_resource_heartbeat fromCapnp(const curious::message::YoutubeResourceHeartbeat::Reader& reader);
  std::string serialize() const;
  static youtube_resource_heartbeat deserialize(const std::string& data);
//#editable_class_start_dont_remove_this_line_only_write_above

//#editable_class_end_dont_remove_this_line_only_write_below
};
}  // namespace curious::net
