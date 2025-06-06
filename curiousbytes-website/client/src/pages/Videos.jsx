import React, { useEffect, useState } from "react";
import VideoCard from "../components/VideoCard";

const VIDEO_TYPES = [
  "GateGeneral",
  "GateStrategy",
  "GateToppersInterview",
];

const VideoTypes = () => {
  const [selectedType, setSelectedType] = useState("GateGeneral");
  const [videos, setVideos] = useState([]);
  const [loading, setLoading] = useState(false);

  const fetchVideos = async (type) => {
    setLoading(true);
    try {
      const res = await fetch(`/api/youtube/videos/${type}`);
      const data = await res.json();
      setVideos(data);
    } catch (err) {
      console.error("Failed to load videos:", err);
    }
    setLoading(false);
  };

  useEffect(() => {
    fetchVideos(selectedType);
  }, [selectedType]);

  return (
    <div className="p-4 max-w-7xl mx-auto">
      <h2 className="text-3xl font-bold mb-6 text-center text-blue-600">🎥 Videos</h2>

      <div className="flex justify-center mb-6 flex-wrap gap-2">
        {VIDEO_TYPES.map((type) => (
          <button
            key={type}
            onClick={() => setSelectedType(type)}
            className={`px-4 py-2 rounded-full border font-medium transition 
              ${
                selectedType === type
                  ? "bg-blue-600 text-white"
                  : "bg-white dark:bg-gray-800 text-blue-600 border-blue-600"
              }`}
          >
            {type}
          </button>
        ))}
      </div>

      {loading ? (
        <p className="text-center text-gray-500">Loading videos...</p>
      ) : (
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
            />
          ))}
        </div>
      )}
    </div>
  );
};

export default VideoTypes;