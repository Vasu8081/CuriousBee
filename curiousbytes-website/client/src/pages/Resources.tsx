import React, { useEffect, useState } from "react";
import {
  MdSchool,
  MdOpenInNew,
  MdPictureAsPdf,
  MdVisibility,
  MdHelp,
} from "react-icons/md";
import { motion, AnimatePresence } from "framer-motion";
import { semanticColors } from "../constants/theme"; // adjust path if needed

interface Resource {
  id: string;
  title: string;
  description?: string;
  type?: string;
  is_featured?: boolean;
  created_at?: string;
  scheduled_at?: string;
  url?: string;
}

const getTypeBadge = (type?: string) => {
  const lower = type?.toLowerCase();
  switch (lower) {
    case "url":
      return {
        label: "External Link",
        icon: <MdOpenInNew />,
        ...semanticColors.badges.url,
      };
    case "pdf":
      return {
        label: "PDF",
        icon: <MdPictureAsPdf />,
        ...semanticColors.badges.pdf,
      };
    case "view":
      return {
        label: "Viewable",
        icon: <MdVisibility />,
        ...semanticColors.badges.view,
      };
    default:
      return {
        label: "Unknown",
        icon: <MdHelp />,
        bg: "bg-gray-100",
        text: "text-gray-700",
      };
  }
};

const Resources: React.FC = () => {
  const [resources, setResources] = useState<Resource[]>([]);
  const [loading, setLoading] = useState(false);

  useEffect(() => {
    const fetchResources = async () => {
      setLoading(true);
      try {
        const res = await fetch("/api/resources");
        const data = await res.json();
        setResources(Array.isArray(data) ? data : []);
      } catch (err) {
        console.error("Failed to load resources:", err);
        setResources([]);
      } finally {
        setLoading(false);
      }
    };
    fetchResources();
  }, []);

  const handleClick = (resource: Resource) => {
    const type = resource.type?.toLowerCase();
    const url = resource.url || "#";

    if (type === "url" || type === "view") {
      window.open(type === "view" ? `/resource/${resource.id}` : url, "_blank");
    } else if (type === "pdf") {
      const link = document.createElement("a");
      link.href = url;
      link.download = "";
      document.body.appendChild(link);
      link.click();
      document.body.removeChild(link);
    }
  };

  return (
    <div className="px-4 py-10 max-w-7xl mx-auto">
      {/* Title */}
      <div className="flex justify-center items-center gap-3 mb-6">
        <MdSchool className="text-3xl text-indigo-600 dark:text-indigo-400" />
        <h2 className="text-3xl font-bold text-gray-900 dark:text-white">Resources</h2>
      </div>

      {/* Content */}
      {loading ? (
        <p className="text-center text-gray-500">Loading resources...</p>
      ) : resources.length === 0 ? (
        <p className="text-center text-gray-500">No resources available.</p>
      ) : (
        <motion.div
          className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-6"
          initial={{ opacity: 0, y: 20 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.4 }}
        >
          <AnimatePresence mode="wait">
            {resources.map((resource) => {
              const badge = getTypeBadge(resource.type);
              return (
                <motion.div
                  key={resource.id}
                  initial={{ opacity: 0, y: 20 }}
                  animate={{ opacity: 1, y: 0 }}
                  exit={{ opacity: 0 }}
                  transition={{ duration: 0.3, delay: 0.05 }}
                >
                  <div
                    onClick={() => handleClick(resource)}
                    className="cursor-pointer bg-white dark:bg-gray-800 shadow-md rounded-xl overflow-hidden border border-gray-200 dark:border-gray-700 hover:scale-105 transition-transform duration-300 p-4"
                  >
                    <h3 className="text-lg font-bold text-blue-600 dark:text-blue-300">
                      {resource.title}
                    </h3>
                    {resource.description && (
                      <p className="text-sm text-gray-600 dark:text-gray-400 mt-1 line-clamp-3">
                        {resource.description}
                      </p>
                    )}
                    {resource.created_at && (
                      <p className="text-xs text-gray-500 dark:text-gray-400 mt-2">
                        {new Date(resource.created_at).toLocaleDateString()}
                      </p>
                    )}
                    <div className="flex flex-wrap items-center gap-2 mt-3">
                      <span
                        className={`inline-flex items-center gap-1 text-xs font-medium px-2 py-0.5 rounded-full ${badge.bg} ${badge.text}`}
                      >
                        {badge.icon} {badge.label}
                      </span>
                      {resource.is_featured && (
                        <span className="inline-block px-2 py-0.5 text-xs bg-yellow-100 text-yellow-700 rounded-full">
                          Featured
                        </span>
                      )}
                    </div>
                  </div>
                </motion.div>
              );
            })}
          </AnimatePresence>
        </motion.div>
      )}
    </div>
  );
};

export default Resources;
