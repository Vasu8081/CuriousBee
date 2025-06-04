import React, { useEffect, useState } from "react";

const GatePrep = () => {
  const [resources, setResources] = useState([]);

  useEffect(() => {
    const fetchResources = async () => {
      try {
        const res = await fetch("/api/youtube/resources");
        const data = await res.json();
        setResources(data);
      } catch (err) {
        console.error("Failed to load resources:", err);
      }
    };

    fetchResources();
  }, []);

  return (
    <div className="p-8">
      <h2 className="text-2xl font-semibold text-blue-700 mb-4">GATE Preparation</h2>
      <p className="mb-6 text-gray-600">Organize notes, PDFs, past year questions, calculators, etc.</p>
      
      <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-6">
        {resources.map((resource, index) => (
          <a
            key={index}
            href={resource.url}
            target="_blank"
            rel="noopener noreferrer"
            className="block p-4 border rounded-lg shadow hover:shadow-md transition bg-white dark:bg-gray-900"
          >
            <h3 className="text-lg font-bold text-blue-600">{resource.label}</h3>
            <p className="text-sm text-gray-500 truncate">{resource.url}</p>
          </a>
        ))}
      </div>
    </div>
  );
};

export default GatePrep;