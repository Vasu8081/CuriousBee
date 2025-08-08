#pragma once
#include <messages/network_msg.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <string>
#include <cstdint>
#include <network/reply.h>
#include <network/youtube_resource.h>
#include <vector>
//#editable_headers_start_dont_remove_this_line_only_write_below

//#editable_headers_end_dont_remove_this_line_only_write_above

namespace curious::net {

class youtube_resource_snapshot_response : public reply {
protected:
  // Properties
  std::vector<youtube_resource> _resources;
public:
  // Constructor
  youtube_resource_snapshot_response() {
    _id = 0;
    _msgType = message_type::youtubeResourceSnapshotResponse;
    _resources = {};
    _topic = "";
  }

  std::vector<youtube_resource> getResources() const { return _resources; }
  void setResources(std::vector<youtube_resource> value) { _resources = value; }

  void toCapnp(curious::message::YoutubeResourceSnapshotResponse::Builder& builder) const;
  static youtube_resource_snapshot_response fromCapnp(const curious::message::YoutubeResourceSnapshotResponse::Reader& reader);
  std::string serialize() const;
  static youtube_resource_snapshot_response deserialize(const std::string& data);
//#editable_class_start_dont_remove_this_line_only_write_above

//#editable_class_end_dont_remove_this_line_only_write_below
};
}  // namespace curious::net
