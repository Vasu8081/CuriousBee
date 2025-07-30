import React from "react";
import { Link } from "react-router-dom";

export default function HeroSection() {
  return (
    <section className="flex flex-col-reverse lg:flex-row items-center justify-between gap-12 px-6 pt-16 pb-20 max-w-7xl mx-auto">
      {/* Left Content */}
      <div className="w-full lg:w-1/2 text-left animate-fade-up">
        <h1 className="text-3xl sm:text-4xl font-bold leading-snug mb-2">
          Crack{" "}
          <span className="text-sky-500 dark:text-sky-400">GATE</span> &{" "}
          <span className="text-violet-500 dark:text-violet-400">Interviews</span>
        </h1>

        <h2 className="text-xl sm:text-2xl font-semibold text-gray-800 dark:text-white mb-6">
          with <span className="italic">Confidence</span> and <span className="italic">Clarity</span>
        </h2>

        <p className="text-base sm:text-lg text-gray-700 dark:text-gray-300 mb-8 max-w-md">
          Get structured guidance for success in competitive exams and top tech roles.
        </p>

        <div className="flex flex-wrap gap-4">
          <Link
            to="/videos?tab=GATE"
            className="bg-sky-500 text-white px-6 py-2 rounded-full text-base font-semibold hover:bg-sky-400 transition"
          >
            Start GATE Prep
          </Link>
          <Link
            to="/videos?tab=Interviews"
            className="bg-violet-500 text-white px-6 py-2 rounded-full text-base font-semibold hover:bg-violet-400 transition"
          >
            Ace Interviews
          </Link>
        </div>
      </div>

      {/* Right Illustration */}
      <div className="w-full lg:w-1/2 flex justify-center animate-fade-up">
        <img
          src="/home-illustrator.png"
          alt="Student illustration"
          className="w-full max-w-xs sm:max-w-sm md:max-w-md"
        />
      </div>
    </section>
  );
}
