import React from "react";
import { FaInstagram, FaLinkedin } from "react-icons/fa";
import { semanticColors, brandColors } from "../constants/theme";

export default function InstructorIntro() {
  return (
    <section className="bg-white dark:bg-gray-900 px-6 py-20 font-sans">
      <div className="max-w-6xl mx-auto flex flex-col md:flex-row items-center justify-between gap-10">

        {/* Left: Text Content */}
        <div className="md:w-1/2 text-left">
          <h2 className="text-3xl md:text-4xl font-bold text-gray-900 dark:text-white mb-6">
            Meet Your Mentor
          </h2>
          <p className="text-base md:text-lg text-gray-700 dark:text-gray-300 leading-relaxed mb-6 relative pl-6">
            <span className="absolute left-0 top-0 text-3xl text-[${brandColors.primary.light}] dark:text-[${brandColors.primary.dark}]">“</span>
            Bhawna is a passionate GATE educator and YouTuber. She holds an MTech from IIT Bombay. Having mentored thousands with her concise teaching style, and currently working at Google, she blends real-world experience with academics.
          </p>

          {/* Socials */}
          <div className="flex gap-6 text-2xl">
            <a
              href="https://instagram.com/_bhawwna"
              target="_blank"
              rel="noopener noreferrer"
              className={`hover:scale-110 transition ${semanticColors.brand.instagram}`}
              aria-label="Instagram"
            >
              <FaInstagram />
            </a>
            <a
              href="https://www.linkedin.com/in/bhawnachelani"
              target="_blank"
              rel="noopener noreferrer"
              className={`hover:scale-110 transition ${semanticColors.brand.linkedin}`}
              aria-label="LinkedIn"
            >
              <FaLinkedin />
            </a>
          </div>
        </div>

        {/* Right: Image & Info */}
        <div className="md:w-1/2 flex flex-col items-center">
          <img
            src="/bhawna.png"
            alt="Bhawna Chelani"
            className="w-[160px] sm:w-[200px] md:w-[220px] h-auto object-cover rounded-full mb-3 shadow-md"
          />
          <div className="text-center text-sm sm:text-base text-gray-800 dark:text-gray-300 bg-gray-100 dark:bg-gray-800 px-4 py-2 rounded-lg shadow-sm">
            Bhawna Chelani, AIR 2 GATE 2022
          </div>
          <div className="mt-3 flex items-center gap-3 text-sm text-gray-600 dark:text-gray-300">
            <span>Bringing work experience from</span>
            <img src="/google.png" alt="Google" className="h-5" />
            <img src="/iitb.png" alt="IIT Bombay" className="h-5 dark:invert dark:brightness-0" />
          </div>
        </div>
      </div>
    </section>
  );
}
