import React, { useEffect, useState } from "react";
import VideoCard from "../components/VideoCard";

const Videos = () => {
  const [videos, setVideos] = useState([]);

  useEffect(() => {
    const fetchFromBackend = async () => {
      try {
        const res = await fetch("/api/youtube/videos");
        const data = await res.json();
        setVideos(data);
      } catch (err) {
        console.error("Failed to load videos from backend:", err);
      }
    };

    fetchFromBackend();
  }, []);

  return (
    <div className="p-4">
      <h1 className="text-3xl font-bold mb-4">Latest YouTube Videos</h1>
      <div className="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-3 gap-6">
        {videos.map((video) => (
          <VideoCard
            key={video.video_id}
            title={video.title}
            videoId={video.video_id}
            thumbnail={
              video.thumbnails?.high?.url ||
              video.thumbnails?.medium?.url ||
              video.thumbnails?.default?.url
            }
            url={`https://www.youtube.com/watch?v=${video.video_id}`}
          />
        ))}
      </div>
    </div>
  );
};

export default Videos;