message NetworkMessage(1) {
    MessageType msgType;
    string topic;
}

message Request(2) extends NetworkMessage {
    int id;
    string reqGeneratedIp;
    string reqGeneratedPort;
}

message Reply(3) extends NetworkMessage {
    int id;
    Request request;
}

message TestRequest(4) extends Request {
    string message;
    string user;
    int age;
}

message TestReply(5) extends Reply {
    string response;
    int responseTest;
}

message YoutubeVideoSnapshotRequest(6) extends Request {

}

message YoutubeVideo(8) extends NetworkMessage {
    string videoId;
    string title;
    string thumbnail;
    string thumbnailMedium;
    string thumbnailHigh;
    string thumbnailStandard;
    string thumbnailMaxres;
}

message YoutubeVideoSnapshotResponse(7) extends Reply {
    list<YoutubeVideo> videos;
}

message YoutubeVideoHeartbeat(9) extends NetworkMessage {
    int videosCount;
}

message YoutubeVideoUpdates(10) extends NetworkMessage {
    list<YoutubeVideo> videos;
}

message YoutubeResourceSnapshotRequest(11) extends Request {

}

message YoutubeResource(12) extends NetworkMessage {
    string resourceId;
    string title;
    string data;
    string description;
}

message YoutubeResourceSnapshotResponse(13) extends Reply {
    list<YoutubeResource> resources;
}

message YoutubeResourceHeartbeat(14) extends NetworkMessage {
    int resourcesCount;
}

message YoutubeResourceUpdates(15) extends NetworkMessage {
    list<YoutubeResource> updates;
}

message YoutubeBlogSnapshotRequest(16) extends Request {

}

message YoutubeBlog(17) extends NetworkMessage {
    string blogId;
    string title;
    string slug;
    string coverImageUrl;
    string publishedDate;
    string contentHtml;
}

message YoutubeBlogSnapshotResponse(18) extends Reply {
    list<YoutubeBlog> blogs;
}

message YoutubeBlogHeartbeat(19) extends NetworkMessage {
    int resourcesCount;
}

message YoutubeBlogUpdates(20) extends NetworkMessage {
    list<YoutubeBlog> updates;
}

