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

class youtube_video_heartbeat : public network_message {
protected:
  // Properties
  int _videosCount;
public:
  // Constructor
  youtube_video_heartbeat() {
    _msgType = message_type::youtubeVideoHeartbeat;
    _topic = "";
    _videosCount = 0;
  }

  int getVideosCount() const { return _videosCount; }
  void setVideosCount(int value) { _videosCount = value; }

  void toCapnp(curious::message::YoutubeVideoHeartbeat::Builder& builder) const;
  static youtube_video_heartbeat fromCapnp(const curious::message::YoutubeVideoHeartbeat::Reader& reader);
  std::string serialize() const;
  static youtube_video_heartbeat deserialize(const std::string& data);
//#editable_class_start_dont_remove_this_line_only_write_above

//#editable_class_end_dont_remove_this_line_only_write_below
};
}  // namespace curious::net
