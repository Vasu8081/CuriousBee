import React, { useState } from "react";

const VideoCard = ({ title, videoId, thumbnail }) => {
  const [isPlaying, setIsPlaying] = useState(false);
  const embedUrl = `https://www.youtube.com/embed/${videoId}?autoplay=1`;

  return (
    <div className="rounded overflow-hidden shadow-md hover:shadow-lg transition-shadow bg-white dark:bg-gray-800">
      <div className="w-full aspect-video cursor-pointer" onClick={() => setIsPlaying(true)}>
        {isPlaying ? (
          <iframe
            src={embedUrl}
            title={title}
            frameBorder="0"
            allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture"
            allowFullScreen
            className="w-full h-full"
          ></iframe>
        ) : (
          <img
            src={thumbnail}
            alt={title}
            className="w-full h-full object-cover"
          />
        )}
      </div>
      <div className="p-4">
        <h2 className="text-lg font-semibold text-gray-800 dark:text-white">{title}</h2>
      </div>
    </div>
  );
};

export default VideoCard;