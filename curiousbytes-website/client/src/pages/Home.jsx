import React from "react";
import { Link } from "react-router-dom";
import InstructorIntro from "../components/InstructorIntro";
import HeroText from "../components/HomeText";
import { FaYoutube, FaLinkedinIn, FaInstagram } from "react-icons/fa";

export default function Home() {
  return (
    <div className="min-h-screen bg-white text-black dark:bg-gray-900 dark:text-white font-sans">
      {/* Hero Section */}
      <section className="flex flex-col items-center justify-center text-center px-6 py-20 bg-[url('/background.jpg')] bg-cover bg-center">
        <HeroText />
        <div className="flex flex-col sm:flex-row gap-4">
          <Link
            to="/videos?tab=GATE"
            className="bg-yellow-400 text-black px-6 py-2 rounded-full text-lg font-semibold hover:bg-yellow-300 transition"
          >
            Crack Gate
          </Link>
          <Link
            to="/videos?tab=Interviews"
            className="bg-indigo-500 text-white px-6 py-2 rounded-full text-lg font-semibold hover:bg-indigo-400 transition"
          >
            Crack Placements
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

      {/* Meet Your Instructor */}
      <InstructorIntro />

      {/* Connect With Us Section */}
      <section className="text-center py-16 px-6 bg-gray-100 dark:bg-gray-800">
        <h2 className="text-3xl sm:text-4xl font-bold mb-10">Connect With Us</h2>
        <div className="grid grid-cols-1 sm:grid-cols-3 gap-6 max-w-6xl mx-auto">
          {/* YouTube */}
          <a
            href="https://www.youtube.com/@curiousbytes67"
            target="_blank"
            rel="noopener noreferrer"
            className="group rounded-2xl p-6 bg-black text-white shadow-xl hover:scale-105 transition transform duration-300 flex flex-col items-center"
            style={{
              background: "linear-gradient(145deg, #1f1f1f, #2b2b2b)",
              boxShadow: "0 8px 20px rgba(0,0,0,0.3)",
            }}
          >
            <p className="text-3xl font-extrabold mb-2">10K+</p>
            <p className="text-lg">YouTube</p>
            <FaYoutube className="text-red-600 text-2xl mt-2 transition duration-300 group-hover:scale-125" />
          </a>

          {/* LinkedIn */}
          <a
            href="https://in.linkedin.com/in/bhawnachelani"
            target="_blank"
            rel="noopener noreferrer"
            className="group rounded-2xl p-6 bg-black text-white shadow-xl hover:scale-105 transition transform duration-300 flex flex-col items-center"
            style={{
              background: "linear-gradient(145deg, #1f1f1f, #2b2b2b)",
              boxShadow: "0 8px 20px rgba(0,0,0,0.3)",
            }}
          >
            <p className="text-3xl font-extrabold mb-2">16K+</p>
            <p className="text-lg">LinkedIn</p>
            <FaLinkedinIn className="text-blue-500 text-2xl mt-2 transition duration-300 group-hover:scale-125" />
          </a>

          {/* Instagram */}
          <a
            href="https://instagram.com/_bhawwna"
            target="_blank"
            rel="noopener noreferrer"
            className="group rounded-2xl p-6 bg-black text-white shadow-xl hover:scale-105 transition transform duration-300 flex flex-col items-center"
            style={{
              background: "linear-gradient(145deg, #1f1f1f, #2b2b2b)",
              boxShadow: "0 8px 20px rgba(0,0,0,0.3)",
            }}
          >
            <p className="text-3xl font-extrabold mb-2">1K+</p>
            <p className="text-lg">Instagram</p>
            <FaInstagram className="text-pink-500 text-2xl mt-2 transition duration-300 group-hover:scale-125" />
          </a>  
        </div>
      </section>
    </div>
  );
}