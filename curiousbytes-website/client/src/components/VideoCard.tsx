import React, { useState } from "react";
import { FaPlay } from "react-icons/fa";
import { semanticColors } from "../constants/theme";

interface VideoCardProps {
  title: string;
  videoId: string;
  thumbnail: string;
}

const VideoCard: React.FC<VideoCardProps> = ({ title, videoId, thumbnail }) => {
  const [isPlaying, setIsPlaying] = useState(false);
  const embedUrl = `https://www.youtube.com/embed/${videoId}?autoplay=1`;

  return (
    <div
      className={`rounded-xl overflow-hidden shadow-md hover:shadow-xl transition-shadow duration-300 
                  ${semanticColors.cards.social} border group`}
      title={title}
    >
      <div
        className="relative w-full aspect-video cursor-pointer"
        onClick={() => setIsPlaying(true)}
      >
        {isPlaying ? (
          <iframe
            src={embedUrl}
            title={title}
            frameBorder="0"
            allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture"
            allowFullScreen
            className="w-full h-full"
          />
        ) : (
          <>
            <img
              src={thumbnail}
              alt="YouTube video thumbnail"
              className="w-full h-full object-cover transition-transform duration-300 group-hover:scale-105 brightness-110"
            />
            <div className="absolute inset-0 flex items-center justify-center">
              <div className="bg-black/50 dark:bg-white/20 rounded-full p-3 transition-all group-hover:scale-110">
                <FaPlay className="text-white text-xl opacity-90" />
              </div>
            </div>
          </>
        )}
      </div>
    </div>
  );
};

export default VideoCard;
