#pragma once
#include <messages/network_msg.capnp.h>
#include <string>
#include <cstdint>
#include <capnp/message.h>
#include <capnp/serialize.h>

#include <network/request.h>


namespace curious::net {

class youtube_video_snapshot_request : public request {
  // #generated start
protected:

public:
  youtube_video_snapshot_request() {
    // Initialize default values if needed
    _id = 0;
    _reqGeneratedIp = "";
    _reqGeneratedPort = "";
     _msgType = message_type::youtubeVideoSnapshotRequest;
  }

  int getId() const { return _id; }
  void setId(int value) { _id = value; }

  const std::string& getReqGeneratedIp() const { return _reqGeneratedIp; }
  void setReqGeneratedIp(const std::string& value) { _reqGeneratedIp = value; }

  const std::string& getReqGeneratedPort() const { return _reqGeneratedPort; }
  void setReqGeneratedPort(const std::string& value) { _reqGeneratedPort = value; }

  // Cap'n Proto conversion functions
  void toCapnp(curious::message::YoutubeVideoSnapshotRequest::Builder& builder) const;
  static youtube_video_snapshot_request fromCapnp(const curious::message::YoutubeVideoSnapshotRequest::Reader& reader);

  // Serialization helpers
  std::string serialize() const;
  static youtube_video_snapshot_request deserialize(const std::string& data);
  // #generated end
};

}  // namespace curious::net
