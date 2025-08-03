#include <network/youtube_video_snapshot_response.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>

namespace curious::net {

void youtube_video_snapshot_response::toCapnp(curious::message::YoutubeVideoSnapshotResponse::Builder& builder) const {
  builder.setMsgType(curious::message::MessageType::YOUTUBE_VIDEO_SNAPSHOT_RESPONSE);
  builder.setId(_id);
  auto RequestBuilder = builder.initRequest();
  _request.toCapnp(RequestBuilder);
  auto VideosBuilder = builder.initVideos(_videos.size());
  for (auto i=0; i<_videos.size(); i++) {
    auto itemBuilder = VideosBuilder[i];
    _videos[i].toCapnp(itemBuilder);
  }
}

youtube_video_snapshot_response youtube_video_snapshot_response::fromCapnp(const curious::message::YoutubeVideoSnapshotResponse::Reader& reader) {
  youtube_video_snapshot_response obj;
  obj._id = reader.getId();
  obj._request = request::fromCapnp(reader.getRequest());
  auto VideosReader = reader.getVideos();
  obj._videos.clear();
  obj._videos.reserve(VideosReader.size());
  for (auto i=0; i<VideosReader.size(); i++) {
    obj._videos[i] =youtube_video::fromCapnp(VideosReader[i]);
  }
  return obj;
}

std::string youtube_video_snapshot_response::serialize() const {
  ::capnp::MallocMessageBuilder message;
  auto builder = message.initRoot<curious::message::YoutubeVideoSnapshotResponse>();
  toCapnp(builder);
  
  kj::VectorOutputStream output;
  capnp::writeMessage(output, message);
  auto data = output.getArray();
  return std::string(reinterpret_cast<const char*>(data.begin()), data.size());
}

youtube_video_snapshot_response youtube_video_snapshot_response::deserialize(const std::string& data) {
  kj::ArrayInputStream input(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(data.data()), data.size()));
  ::capnp::InputStreamMessageReader message(input);
  auto reader = message.getRoot<curious::message::YoutubeVideoSnapshotResponse>();
  return fromCapnp(reader);
}

}  // namespace curious::net
