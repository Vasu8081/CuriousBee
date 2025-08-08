#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <network/reply.h>
#include <network/youtube_video.h>
#include <vector>
//#editable_headers_start_dont_remove_this_line_only_write_below

//#editable_headers_end_dont_remove_this_line_only_write_above

namespace curious::net {

class youtube_video_snapshot_response : public reply {
protected:
  // Properties
  std::vector<youtube_video> _videos;
public:
  // Constructor
  youtube_video_snapshot_response() {
    _id = 0;
    _msgType = message_type::youtubeVideoSnapshotResponse;
    _topic = "";
    _videos = {};
  }

  std::vector<youtube_video> getVideos() const { return _videos; }
  void setVideos(std::vector<youtube_video> value) { _videos = value; }

  void toCapnp(curious::message::YoutubeVideoSnapshotResponse::Builder& builder) const;
  static youtube_video_snapshot_response fromCapnp(const curious::message::YoutubeVideoSnapshotResponse::Reader& reader);
  std::string serialize() const;
  static youtube_video_snapshot_response deserialize(const std::string& data);
//#editable_class_start_dont_remove_this_line_only_write_above

//#editable_class_end_dont_remove_this_line_only_write_below
};
}  // namespace curious::net
