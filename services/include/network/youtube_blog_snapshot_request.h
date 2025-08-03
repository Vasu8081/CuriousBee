#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <network/request.h>

namespace curious::net {

class youtube_blog_snapshot_request : public request {
protected:
  // Properties
public:
  // Constructor
  youtube_blog_snapshot_request() {
    _id = 0;
    _msgType = message_type::youtubeBlogSnapshotRequest;
    _reqGeneratedIp = "";
    _reqGeneratedPort = "";
    _topic = "";
  }

  void toCapnp(curious::message::YoutubeBlogSnapshotRequest::Builder& builder) const;
  static youtube_blog_snapshot_request fromCapnp(const curious::message::YoutubeBlogSnapshotRequest::Reader& reader);
  std::string serialize() const;
  static youtube_blog_snapshot_request deserialize(const std::string& data);
};
}  // namespace curious::net
