#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <network/network_message.h>
#include <network/youtube_blog.h>
#include <vector>
//#editable_headers_start_dont_remove_this_line_only_write_below

//#editable_headers_end_dont_remove_this_line_only_write_above

namespace curious::net {

class youtube_blog_updates : public network_message {
protected:
  // Properties
  std::vector<youtube_blog> _updates;
public:
  // Constructor
  youtube_blog_updates() {
    _msgType = message_type::youtubeBlogUpdates;
    _topic = "";
    _updates = {};
  }

  std::vector<youtube_blog> getUpdates() const { return _updates; }
  void setUpdates(std::vector<youtube_blog> value) { _updates = value; }

  void toCapnp(curious::message::YoutubeBlogUpdates::Builder& builder) const;
  static youtube_blog_updates fromCapnp(const curious::message::YoutubeBlogUpdates::Reader& reader);
  std::string serialize() const;
  static youtube_blog_updates deserialize(const std::string& data);
//#editable_class_start_dont_remove_this_line_only_write_above

//#editable_class_end_dont_remove_this_line_only_write_below
};
}  // namespace curious::net
