import React, { useEffect, useState } from "react";

const HeroText = () => {
  const [showGate, setShowGate] = useState(true);

  useEffect(() => {
    const interval = setInterval(() => {
      setShowGate((prev) => !prev);
    }, 1800);

    return () => clearInterval(interval);
  }, []);

  return (
    <div className="text-center mb-4">
      <div className="inline-block text-left">
        <p className="text-4xl sm:text-5xl font-extrabold leading-tight mb-1">
          <span className="mr-2">Crack</span>
          <span
            className={`inline-block w-[10ch] transition-colors duration-300 ${
              showGate ? "text-yellow-400" : "text-indigo-400"
            }`}
          >
            {showGate ? "GATE" : "Placements"}
          </span>
        </p>
        <p className="text-xl sm:text-2xl font-medium text-gray-300 dark:text-gray-400">
          with Confidence and Clarity
        </p>
      </div>
    </div>
  );
};

export default HeroText;