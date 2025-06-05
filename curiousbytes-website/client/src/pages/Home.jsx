import React from "react";
import { Link } from "react-router-dom";

export default function Home() {
  return (
    <div className="min-h-screen bg-white text-black dark:bg-gray-900 dark:text-white font-sans">
      {/* Hero Section */}
      <section className="flex flex-col items-center justify-center text-center px-6 py-20 bg-[url('/background.jpg')] bg-cover bg-center">
        <h1 className="text-4xl sm:text-5xl font-extrabold leading-tight mb-4">
          Stay Curious and Keep Progressing
        </h1>
        <p className="text-lg sm:text-xl mb-6">
          GATE | CODING | PLACEMENT | Life @ IIT
        </p>

        <div className="flex flex-col sm:flex-row gap-4">
          <Link
            to="/videos"
            className="bg-yellow-400 text-black px-6 py-2 rounded-full text-lg font-semibold hover:bg-yellow-300 transition"
          >
            Watch Videos
          </Link>
          <Link
            to="/keypoints"
            className="bg-indigo-500 text-white px-6 py-2 rounded-full text-lg font-semibold hover:bg-indigo-400 transition"
          >
            Watch Key Moments
          </Link>
        </div>
      </section>

      {/* Sections */}
      <section className="grid grid-cols-1 sm:grid-cols-3 gap-6 px-6 py-12 text-center bg-white dark:bg-gray-900">
        <Link to="/gate" className="bg-gray-100 dark:bg-gray-800 rounded-xl p-6 hover:shadow-lg hover:bg-gray-200 dark:hover:bg-gray-700">
          <h2 className="text-2xl font-bold mb-2">GATE Prep</h2>
          <p>Master core subjects with structured notes and practice.</p>
        </Link>
        <Link to="/interviews" className="bg-gray-100 dark:bg-gray-800 rounded-xl p-6 hover:shadow-lg hover:bg-gray-200 dark:hover:bg-gray-700">
          <h2 className="text-2xl font-bold mb-2">Interview Prep</h2>
          <p>Crack top companies with coding & system design guidance.</p>
        </Link>
        <Link to="/videos" className="bg-gray-100 dark:bg-gray-800 rounded-xl p-6 hover:shadow-lg hover:bg-gray-200 dark:hover:bg-gray-700">
          <h2 className="text-2xl font-bold mb-2">YouTube Content</h2>
          <p>Explore engaging videos on all things tech and IIT life.</p>
        </Link>
      </section>
    </div>
  );
}