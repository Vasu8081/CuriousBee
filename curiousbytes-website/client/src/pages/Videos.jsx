import React, { useEffect, useState } from "react";
import { useLocation } from "react-router-dom";
import VideoCard from "../components/VideoCard.tsx";
import { PiVideoDuotone } from "react-icons/pi";
import { motion, AnimatePresence } from "framer-motion";
import { brandColors } from "../constants/theme";

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

export default function Videos() {
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
    <div className="px-4 py-10 max-w-7xl mx-auto">
      {/* Title */}
      <div className="flex justify-center items-center gap-3 mb-6">
        <PiVideoDuotone className="text-3xl text-indigo-600 dark:text-indigo-400" />
        <h2 className="text-3xl font-bold text-gray-900 dark:text-white">Videos</h2>
      </div>

      {/* Tab Buttons */}
      <div className="flex justify-center gap-4 mb-10">
        {Object.values(TABS).map((tab) => {
          const isActive = selectedTab === tab;
          const isGate = tab === "GATE";
          const primaryColor = brandColors.primary[isGate ? "gate" : "interview"];
          const hoverColor = brandColors.hover[isGate ? "gate" : "interview"];

          return (
            <button
              key={tab}
              onClick={() => setSelectedTab(tab)}
              className={`px-6 py-2 rounded-full font-semibold text-base border transition-all duration-300 ease-in-out hover:scale-105`}
              style={{
                backgroundColor: isActive ? primaryColor : "transparent",
                color: isActive ? "#ffffff" : primaryColor,
                borderColor: primaryColor,
              }}
              onMouseOver={(e) => {
                if (!isActive) {
                  e.currentTarget.style.backgroundColor = hoverColor;
                  e.currentTarget.style.color = "#ffffff";
                }
              }}
              onMouseOut={(e) => {
                if (!isActive) {
                  e.currentTarget.style.backgroundColor = "transparent";
                  e.currentTarget.style.color = primaryColor;
                }
              }}
            >
              {tab}
            </button>
          );
        })}
      </div>

      {/* Videos */}
      {loading ? (
        <p className="text-center text-gray-500">Loading videos...</p>
      ) : videos.length === 0 ? (
        <p className="text-center text-gray-500">No videos present.</p>
      ) : (
        <motion.div
          key={selectedTab}
          className="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-3 gap-6"
          initial={{ opacity: 0, y: 20 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.4 }}
        >
          <AnimatePresence mode="wait">
            {videos.map((video) => (
              <motion.div
                key={video.video_id}
                initial={{ opacity: 0, y: 20 }}
                animate={{ opacity: 1, y: 0 }}
                exit={{ opacity: 0 }}
                transition={{ duration: 0.3, delay: 0.05 }}
              >
                <VideoCard
                  title={video.title}
                  videoId={video.video_id}
                  thumbnail={
                    video.thumbnails?.high?.url ||
                    video.thumbnails?.medium?.url ||
                    video.thumbnails?.default?.url
                  }
                />
              </motion.div>
            ))}
          </AnimatePresence>
        </motion.div>
      )}
    </div>
  );
}
