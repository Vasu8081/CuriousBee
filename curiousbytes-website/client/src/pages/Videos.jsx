import React, { useEffect, useState } from "react";
import { useLocation } from "react-router-dom";
import VideoCard from "../components/VideoCard";

const TABS = {
  GATE: "GATE",
  INTERVIEWS: "Interviews",
};

const apiEndpoints = {
  [TABS.GATE]: "/api/youtube/gate/videos",
  [TABS.INTERVIEWS]: "/api/youtube/interviews/videos",
};

function useQuery() {
  return new URLSearchParams(useLocation().search);
}

const Videos = () => {
  const query = useQuery();
  const rawTab = query.get("tab");
  const isValidTab = Object.values(TABS).includes(rawTab);
  const [selectedTab, setSelectedTab] = useState(isValidTab ? rawTab : TABS.GATE);
  const [videos, setVideos] = useState([]);
  const [loading, setLoading] = useState(false);

  const fetchVideos = async () => {
    setLoading(true);
    try {
      const res = await fetch(apiEndpoints[selectedTab]);
      const data = await res.json();
      setVideos(data || []);
    } catch (err) {
      console.error("Failed to load videos:", err);
      setVideos([]);
    }
    setLoading(false);
  };

  useEffect(() => {
    fetchVideos();
  }, [selectedTab]);

  return (
    <div className="p-4 max-w-7xl mx-auto">
      <h2 className="text-3xl font-bold mb-6 text-center text-blue-600">🎥 Videos</h2>

      <div className="flex justify-center gap-4 mb-6">
        {Object.values(TABS).map((tab) => (
          <button
            key={tab}
            onClick={() => setSelectedTab(tab)}
            className={`px-4 py-2 rounded-full border transition font-semibold ${
              selectedTab === tab
                ? "bg-blue-600 text-white border-blue-600"
                : "bg-white dark:bg-gray-800 text-blue-600 border-blue-600"
            }`}
          >
            {tab}
          </button>
        ))}
      </div>

      {loading ? (
        <p className="text-center text-gray-500">Loading videos...</p>
      ) : videos.length === 0 ? (
        <p className="text-center text-gray-500">No videos present.</p>
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

export default Videos;