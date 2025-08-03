#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <network/network_message.h>
#include <string>
//#editable_headers_start_dont_remove_this_line_only_write_below

//#editable_headers_end_dont_remove_this_line_only_write_above

namespace curious::net {

class youtube_video : public network_message {
protected:
  // Properties
  std::string _videoId;
  std::string _title;
  std::string _thumbnail;
  std::string _thumbnailMedium;
  std::string _thumbnailHigh;
  std::string _thumbnailStandard;
  std::string _thumbnailMaxres;
public:
  // Constructor
  youtube_video() {
    _msgType = message_type::youtubeVideo;
    _thumbnail = "";
    _thumbnailHigh = "";
    _thumbnailMaxres = "";
    _thumbnailMedium = "";
    _thumbnailStandard = "";
    _title = "";
    _topic = "";
    _videoId = "";
  }

  std::string getVideoId() const { return _videoId; }
  void setVideoId(std::string value) { _videoId = value; }

  std::string getTitle() const { return _title; }
  void setTitle(std::string value) { _title = value; }

  std::string getThumbnail() const { return _thumbnail; }
  void setThumbnail(std::string value) { _thumbnail = value; }

  std::string getThumbnailMedium() const { return _thumbnailMedium; }
  void setThumbnailMedium(std::string value) { _thumbnailMedium = value; }

  std::string getThumbnailHigh() const { return _thumbnailHigh; }
  void setThumbnailHigh(std::string value) { _thumbnailHigh = value; }

  std::string getThumbnailStandard() const { return _thumbnailStandard; }
  void setThumbnailStandard(std::string value) { _thumbnailStandard = value; }

  std::string getThumbnailMaxres() const { return _thumbnailMaxres; }
  void setThumbnailMaxres(std::string value) { _thumbnailMaxres = value; }

  void toCapnp(curious::message::YoutubeVideo::Builder& builder) const;
  static youtube_video fromCapnp(const curious::message::YoutubeVideo::Reader& reader);
  std::string serialize() const;
  static youtube_video deserialize(const std::string& data);
//#editable_class_start_dont_remove_this_line_only_write_above

//#editable_class_end_dont_remove_this_line_only_write_below
};
}  // namespace curious::net
