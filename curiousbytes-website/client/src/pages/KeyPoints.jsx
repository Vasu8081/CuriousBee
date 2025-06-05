import React, { useEffect, useState } from "react";

export default function KeyPoints() {
  const [shorts, setShorts] = useState([]);

  useEffect(() => {
    const fetchShorts = async () => {
      try {
        const res = await fetch("/api/youtube/shorts");
        const data = await res.json();
        setShorts(data);
      } catch (error) {
        console.error("Failed to load shorts:", error);
      }
    };

    fetchShorts();
  }, []);

  return (
    <div className="p-6 max-w-7xl mx-auto">
      <h2 className="text-3xl font-bold mb-6 text-center text-blue-600">
        📌 Key Points (Shorts)
      </h2>

      {shorts.length === 0 ? (
        <p className="text-center text-gray-500">No shorts available.</p>
      ) : (
        <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-6">
          {shorts.map((short) => (
            <div
              key={short.video_id}
              className="rounded-lg shadow hover:shadow-lg transition overflow-hidden bg-white dark:bg-gray-800"
            >
              {/* Vertical 9:16 aspect ratio for Shorts */}
              <div className="aspect-[9/16]">
                <iframe
                  className="w-full h-full"
                  src={`https://www.youtube.com/embed/${short.video_id}`}
                  title={short.title}
                  frameBorder="0"
                  allowFullScreen
                  allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture"
                ></iframe>
              </div>
              <div className="p-3">
                <h3 className="text-base font-semibold text-gray-800 dark:text-white">
                  {short.title}
                </h3>
              </div>
            </div>
          ))}
        </div>
      )}
    </div>
  );
}