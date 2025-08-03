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

