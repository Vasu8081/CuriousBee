import React from "react";
import { FaInstagram, FaLinkedin, FaXTwitter } from "react-icons/fa6";

export default function InstructorIntro() {
  return (
    <section className="py-20 bg-white text-center px-4">
      {/* Title */}
      <p className="text-orange-500 font-semibold mb-2">Meet your Mentor</p>
      <h2 className="text-3xl md:text-4xl font-bold mb-10">
        The Architects of Your Success
      </h2>

      {/* Main content */}
      <div className="max-w-5xl mx-auto flex flex-col md:flex-row items-center gap-12">
        {/* Left: Bio + Social */}
        <div className="md:w-1/2 text-left space-y-6">
          <p className="text-gray-700 text-base md:text-lg relative pl-6">
            <span className="absolute left-0 text-3xl text-orange-500 top-[-4px]">“</span>
            Bhawna is a passionate GATE Educator, Full Stack Developer, and YouTuber. She holds an MTech from IIT Bombay. She's mentored thousands of students with practical, concise teaching. Currently working at Google, she brings real-world experience to her teaching.
          </p>

          {/* Social Links */}
          <div className="flex gap-5 pt-2">
            <a
              href="https://instagram.com/bhawna"
              target="_blank"
              rel="noopener noreferrer"
              className="text-gray-600 hover:text-orange-500 text-xl"
              aria-label="Instagram"
            >
              <FaInstagram />
            </a>
            <a
              href="https://www.linkedin.com/in/bhawnachelani"
              target="_blank"
              rel="noopener noreferrer"
              className="text-gray-600 hover:text-orange-500 text-xl"
              aria-label="LinkedIn"
            >
              <FaLinkedin />
            </a>
            <a
              href="https://x.com/bhawna"
              target="_blank"
              rel="noopener noreferrer"
              className="text-gray-600 hover:text-orange-500 text-xl"
              aria-label="Twitter"
            >
              <FaXTwitter />
            </a>
          </div>
        </div>

        {/* Right: Profile photo */}
        <div className="md:w-1/2 flex justify-center relative">
          <div className="relative z-10">
            <img
              src="/bhawna.png"
              alt="Bhawna Chelani"
              className="rounded-full w-64 h-64 object-cover border-4 border-white shadow-lg"
            />
          </div>
          <div className="absolute z-0 w-72 h-72 bg-orange-300 rounded-full -bottom-6 -left-6"></div>
        </div>
      </div>

      {/* Work Experience Logos */}
      <div className="mt-16 bg-white p-4 shadow-sm rounded-lg max-w-xl mx-auto">
        <p className="text-sm text-gray-500 mb-3">Bringing work experience from</p>
        <div className="flex justify-center items-center gap-6 flex-wrap">
          <img src="/google.png" alt="Google" className="h-6" />
          <img src="/iitb.png" alt="IITBombay" className="h-6" />
        </div>
      </div>
    </section>
  );
}