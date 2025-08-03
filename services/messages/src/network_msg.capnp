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
  youtubeVideo @8;
  youtubeVideoSnapshotRequest @6;
  youtubeVideoSnapshotResponse @7;
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

