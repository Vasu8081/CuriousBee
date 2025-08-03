#pragma once
#include <messages/network_msg.capnp.h>
#include <string>
#include <cstdint>
#include <capnp/message.h>
#include <capnp/serialize.h>

#include <network/network_message.h>

#include <network/message_type.h>

namespace curious::net {

class youtube_video : public network_message {
  // #generated start
protected:
  std::string _videoId;
  std::string _title;
  std::string _thumbnail;
  std::string _thumbnailMedium;
  std::string _thumbnailHigh;
  std::string _thumbnailStandard;
  std::string _thumbnailMaxres;

public:
  youtube_video() {
    // Initialize default values if needed
    // Initialize custom type message_type if needed
    _topic = "";
    _videoId = "";
    _title = "";
    _thumbnail = "";
    _thumbnailMedium = "";
    _thumbnailHigh = "";
    _thumbnailStandard = "";
    _thumbnailMaxres = "";
     _msgType = message_type::youtubeVideo;
  }

  message_type getMsgType() const { return _msgType; }
  void setMsgType(message_type value) { _msgType = value; }

  const std::string& getTopic() const { return _topic; }
  void setTopic(const std::string& value) { _topic = value; }

  const std::string& getVideoId() const { return _videoId; }
  void setVideoId(const std::string& value) { _videoId = value; }

  const std::string& getTitle() const { return _title; }
  void setTitle(const std::string& value) { _title = value; }

  const std::string& getThumbnail() const { return _thumbnail; }
  void setThumbnail(const std::string& value) { _thumbnail = value; }

  const std::string& getThumbnailMedium() const { return _thumbnailMedium; }
  void setThumbnailMedium(const std::string& value) { _thumbnailMedium = value; }

  const std::string& getThumbnailHigh() const { return _thumbnailHigh; }
  void setThumbnailHigh(const std::string& value) { _thumbnailHigh = value; }

  const std::string& getThumbnailStandard() const { return _thumbnailStandard; }
  void setThumbnailStandard(const std::string& value) { _thumbnailStandard = value; }

  const std::string& getThumbnailMaxres() const { return _thumbnailMaxres; }
  void setThumbnailMaxres(const std::string& value) { _thumbnailMaxres = value; }

  // Cap'n Proto conversion functions
  void toCapnp(curious::message::YoutubeVideo::Builder& builder) const;
  static youtube_video fromCapnp(const curious::message::YoutubeVideo::Reader& reader);

  // Serialization helpers
  std::string serialize() const;
  static youtube_video deserialize(const std::string& data);
  // #generated end
};

}  // namespace curious::net
