#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <network/network_message.h>
#include <network/youtube_video.h>
#include <vector>

namespace curious::net {

class youtube_video_updates : public network_message {
protected:
  // Properties
  std::vector<youtube_video> _videos;
public:
  // Constructor
  youtube_video_updates() {
    _msgType = message_type::youtubeVideoUpdates;
    _topic = "";
    _videos = {};
  }

  std::vector<youtube_video> getVideos() const { return _videos; }
  void setVideos(std::vector<youtube_video> value) { _videos = value; }

  void toCapnp(curious::message::YoutubeVideoUpdates::Builder& builder) const;
  static youtube_video_updates fromCapnp(const curious::message::YoutubeVideoUpdates::Reader& reader);
  std::string serialize() const;
  static youtube_video_updates deserialize(const std::string& data);
};
}  // namespace curious::net
