#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <network/reply.h>
#include <network/youtube_blog.h>
#include <vector>

namespace curious::net {

class youtube_blog_snapshot_response : public reply {
protected:
  // Properties
  std::vector<youtube_blog> _blogs;
public:
  // Constructor
  youtube_blog_snapshot_response() {
    _blogs = {};
    _id = 0;
    _msgType = message_type::youtubeBlogSnapshotResponse;
    _topic = "";
  }

  std::vector<youtube_blog> getBlogs() const { return _blogs; }
  void setBlogs(std::vector<youtube_blog> value) { _blogs = value; }

  void toCapnp(curious::message::YoutubeBlogSnapshotResponse::Builder& builder) const;
  static youtube_blog_snapshot_response fromCapnp(const curious::message::YoutubeBlogSnapshotResponse::Reader& reader);
  std::string serialize() const;
  static youtube_blog_snapshot_response deserialize(const std::string& data);
};
}  // namespace curious::net
