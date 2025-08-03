#pragma once
#include <messages/network_msg.capnp.h>
#include <string>
#include <cstdint>
#include <capnp/message.h>
#include <capnp/serialize.h>

#include <network/reply.h>
#include <vector>
#include <network/request.h>
#include <network/youtube_video.h>

namespace curious::net {

class youtube_video_snapshot_response : public reply {
  // #generated start
protected:
  std::vector<youtube_video> _videos;

public:
  youtube_video_snapshot_response() {
    // Initialize default values if needed
    _id = 0;
    // Initialize custom type request if needed
    // Initialize custom type std::vector<youtube_video> if needed
     _msgType = message_type::youtubeVideoSnapshotResponse;
  }

  int getId() const { return _id; }
  void setId(int value) { _id = value; }

  request getRequest() const { return _request; }
  void setRequest(request value) { _request = value; }

  std::vector<youtube_video> getVideos() const { return _videos; }
  void setVideos(std::vector<youtube_video> value) { _videos = value; }

  // Cap'n Proto conversion functions
  void toCapnp(curious::message::YoutubeVideoSnapshotResponse::Builder& builder) const;
  static youtube_video_snapshot_response fromCapnp(const curious::message::YoutubeVideoSnapshotResponse::Reader& reader);

  // Serialization helpers
  std::string serialize() const;
  static youtube_video_snapshot_response deserialize(const std::string& data);
  // #generated end
};

}  // namespace curious::net
