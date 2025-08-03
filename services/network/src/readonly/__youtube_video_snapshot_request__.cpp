#include <network/youtube_video_snapshot_request.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>

namespace curious::net {

void youtube_video_snapshot_request::toCapnp(curious::message::YoutubeVideoSnapshotRequest::Builder& builder) const {
  builder.setMsgType(curious::message::MessageType::YOUTUBE_VIDEO_SNAPSHOT_REQUEST);
  builder.setId(_id);
  builder.setReqGeneratedIp(_reqGeneratedIp);
  builder.setReqGeneratedPort(_reqGeneratedPort);
}

youtube_video_snapshot_request youtube_video_snapshot_request::fromCapnp(const curious::message::YoutubeVideoSnapshotRequest::Reader& reader) {
  youtube_video_snapshot_request obj;
  obj._id = reader.getId();
  obj._reqGeneratedIp = reader.getReqGeneratedIp();
  obj._reqGeneratedPort = reader.getReqGeneratedPort();
  return obj;
}

std::string youtube_video_snapshot_request::serialize() const {
  ::capnp::MallocMessageBuilder message;
  auto builder = message.initRoot<curious::message::YoutubeVideoSnapshotRequest>();
  toCapnp(builder);
  
  kj::VectorOutputStream output;
  capnp::writeMessage(output, message);
  auto data = output.getArray();
  return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

youtube_video_snapshot_request youtube_video_snapshot_request::deserialize(const std::string& data) {
  kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
  ::capnp::InputStreamMessageReader message(input);
  auto reader = message.getRoot<curious::message::YoutubeVideoSnapshotRequest>();
  return fromCapnp(reader);
}

}  // namespace curious::net
