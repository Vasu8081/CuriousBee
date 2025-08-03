@0xecd3d69fdf10c2c3;
using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("curious::message");

enum MessageType {
  unknown @0;
  networkMessage @1;
  reply @3;
  request @2;
  testReply @5;
  testRequest @4;
  youtubeBlog @17;
  youtubeBlogHeartbeat @19;
  youtubeBlogSnapshotRequest @16;
  youtubeBlogSnapshotResponse @18;
  youtubeBlogUpdates @20;
  youtubeResource @12;
  youtubeResourceHeartbeat @14;
  youtubeResourceSnapshotRequest @11;
  youtubeResourceSnapshotResponse @13;
  youtubeResourceUpdates @15;
  youtubeVideo @8;
  youtubeVideoHeartbeat @9;
  youtubeVideoSnapshotRequest @6;
  youtubeVideoSnapshotResponse @7;
  youtubeVideoUpdates @10;
}

struct Map(Key, Value) {
  entries @0 :List(Entry);
  struct Entry {
    key @0 :Key;
    value @1 :Value;
  }
}

struct NetworkMessage {
  msgType @0 : MessageType;
  topic @1 : Text;
}

struct Reply {
  msgType @0 : MessageType;
  topic @1 : Text;
  id @2 : Int32;
  request @3 : Request;
}

struct Request {
  msgType @0 : MessageType;
  topic @1 : Text;
  id @2 : Int32;
  reqGeneratedIp @3 : Text;
  reqGeneratedPort @4 : Text;
}

struct TestReply {
  msgType @0 : MessageType;
  topic @1 : Text;
  id @2 : Int32;
  request @3 : Request;
  response @4 : Text;
  responseTest @5 : Int32;
}

struct TestRequest {
  msgType @0 : MessageType;
  topic @1 : Text;
  id @2 : Int32;
  reqGeneratedIp @3 : Text;
  reqGeneratedPort @4 : Text;
  message @5 : Text;
  user @6 : Text;
  age @7 : Int32;
}

struct YoutubeBlog {
  msgType @0 : MessageType;
  topic @1 : Text;
  blogId @2 : Text;
  title @3 : Text;
  slug @4 : Text;
  coverImageUrl @5 : Text;
  publishedDate @6 : Text;
  contentHtml @7 : Text;
}

struct YoutubeBlogHeartbeat {
  msgType @0 : MessageType;
  topic @1 : Text;
  resourcesCount @2 : Int32;
}

struct YoutubeBlogSnapshotRequest {
  msgType @0 : MessageType;
  topic @1 : Text;
  id @2 : Int32;
  reqGeneratedIp @3 : Text;
  reqGeneratedPort @4 : Text;
}

struct YoutubeBlogSnapshotResponse {
  msgType @0 : MessageType;
  topic @1 : Text;
  id @2 : Int32;
  request @3 : Request;
  blogs @4 : List(YoutubeBlog);
}

struct YoutubeBlogUpdates {
  msgType @0 : MessageType;
  topic @1 : Text;
  updates @2 : List(YoutubeBlog);
}

struct YoutubeResource {
  msgType @0 : MessageType;
  topic @1 : Text;
  resourceId @2 : Text;
  title @3 : Text;
  data @4 : Text;
  description @5 : Text;
}

struct YoutubeResourceHeartbeat {
  msgType @0 : MessageType;
  topic @1 : Text;
  resourcesCount @2 : Int32;
}

struct YoutubeResourceSnapshotRequest {
  msgType @0 : MessageType;
  topic @1 : Text;
  id @2 : Int32;
  reqGeneratedIp @3 : Text;
  reqGeneratedPort @4 : Text;
}

struct YoutubeResourceSnapshotResponse {
  msgType @0 : MessageType;
  topic @1 : Text;
  id @2 : Int32;
  request @3 : Request;
  resources @4 : List(YoutubeResource);
}

struct YoutubeResourceUpdates {
  msgType @0 : MessageType;
  topic @1 : Text;
  updates @2 : List(YoutubeResource);
}

struct YoutubeVideo {
  msgType @0 : MessageType;
  topic @1 : Text;
  videoId @2 : Text;
  title @3 : Text;
  thumbnail @4 : Text;
  thumbnailMedium @5 : Text;
  thumbnailHigh @6 : Text;
  thumbnailStandard @7 : Text;
  thumbnailMaxres @8 : Text;
}

struct YoutubeVideoHeartbeat {
  msgType @0 : MessageType;
  topic @1 : Text;
  videosCount @2 : Int32;
}

struct YoutubeVideoSnapshotRequest {
  msgType @0 : MessageType;
  topic @1 : Text;
  id @2 : Int32;
  reqGeneratedIp @3 : Text;
  reqGeneratedPort @4 : Text;
}

struct YoutubeVideoSnapshotResponse {
  msgType @0 : MessageType;
  topic @1 : Text;
  id @2 : Int32;
  request @3 : Request;
  videos @4 : List(YoutubeVideo);
}

struct YoutubeVideoUpdates {
  msgType @0 : MessageType;
  topic @1 : Text;
  videos @2 : List(YoutubeVideo);
}

