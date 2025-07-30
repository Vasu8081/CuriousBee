import React from "react";
import { FaYoutube, FaLinkedin, FaInstagram } from "react-icons/fa";

function SocialCard({ platform, count, icon, link }) {
  return (
    <a
      href={link}
      target="_blank"
      rel="noopener noreferrer"
      className="group w-full p-6 rounded-xl border bg-white dark:bg-gray-800 
                 border-gray-200 dark:border-gray-700 transition-transform transform 
                 hover:scale-[1.03] hover:shadow-lg dark:hover:shadow-2xl 
                 duration-300 ease-in-out flex flex-col items-center justify-center text-center"
    >
      <div className="text-3xl font-extrabold text-gray-900 dark:text-white mb-1">
        {count}
      </div>
      <div className="text-base font-medium text-gray-700 dark:text-gray-300 mb-2">
        {platform}
      </div>
      <div className="text-2xl transition-transform duration-300 group-hover:scale-110">
        {icon}
      </div>
    </a>
  );
}

export default function ConnectWithUs() {
  return (
    <section className="py-20 px-6 bg-white dark:bg-gray-900 text-center font-sans">
      <h2 className="text-3xl md:text-4xl font-bold mb-12 text-gray-800 dark:text-white">
        Connect With Us
      </h2>

      <div className="grid grid-cols-1 sm:grid-cols-3 gap-6 max-w-6xl mx-auto">
        <SocialCard
          platform="YouTube"
          count="11K+"
          icon={<FaYoutube className="text-[#FF0000]" />} // Official YouTube red
          link="https://www.youtube.com/@curiousbytes67"
        />
        <SocialCard
          platform="LinkedIn"
          count="17K+"
          icon={<FaLinkedin className="text-[#0077B5]" />} // Official LinkedIn blue
          link="https://in.linkedin.com/in/bhawnachelani"
        />
        <SocialCard
          platform="Instagram"
          count="1K+"
          icon={<FaInstagram className="text-[#E1306C]" />} // Instagram brand pink
          link="https://instagram.com/_bhawwna"
        />
      </div>
    </section>
  );
}
