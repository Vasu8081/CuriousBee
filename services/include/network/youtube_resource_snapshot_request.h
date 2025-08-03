#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <network/request.h>

namespace curious::net {

class youtube_resource_snapshot_request : public request {
protected:
  // Properties
public:
  // Constructor
  youtube_resource_snapshot_request() {
    _id = 0;
    _msgType = message_type::youtubeResourceSnapshotRequest;
    _reqGeneratedIp = "";
    _reqGeneratedPort = "";
    _topic = "";
  }

  void toCapnp(curious::message::YoutubeResourceSnapshotRequest::Builder& builder) const;
  static youtube_resource_snapshot_request fromCapnp(const curious::message::YoutubeResourceSnapshotRequest::Reader& reader);
  std::string serialize() const;
  static youtube_resource_snapshot_request deserialize(const std::string& data);
};
}  // namespace curious::net
