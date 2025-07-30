import React from "react";
import {
  PiRoadHorizonDuotone,
  PiStudentDuotone,
  PiBrainDuotone,
} from "react-icons/pi";

const features = [
  {
    icon: PiRoadHorizonDuotone,
    title: "Structured Roadmaps",
    description: "Clear step-by-step plans tailored for GATE and tech interviews.",
  },
  {
    icon: PiStudentDuotone,
    title: "Top Mentors",
    description: "Learn from IITians, top GATE rankers, and industry professionals.",
  },
  {
    icon: PiBrainDuotone,
    title: "Real-world Practice",
    description: "Mock tests, DSA coding, system design, and past year questions.",
  },
];

export default function WhyCuriousBytes() {
  return (
    <section className="py-20 px-6 bg-white dark:bg-gray-900 text-center font-sans">
      <h2 className="text-3xl md:text-4xl font-bold mb-14 text-gray-900 dark:text-white">
        Why Curious Bytes?
      </h2>

      <div className="grid grid-cols-1 sm:grid-cols-3 gap-10 max-w-6xl mx-auto">
        {features.map((feature, index) => {
          const Icon = feature.icon;
          return (
            <div
              key={index}
              className="group p-6 rounded-xl border bg-white dark:bg-gray-800 
                         border-gray-200 dark:border-gray-700 shadow-md 
                         transition-all duration-300 text-left transform 
                         hover:scale-[1.03] cursor-pointer"
            >
              <Icon className="text-indigo-600 dark:text-indigo-400 text-4xl mb-4 transition-transform group-hover:scale-110" />
              <h3 className="text-xl font-semibold text-gray-900 dark:text-white mb-2">
                {feature.title}
              </h3>
              <p
                className="text-sm text-gray-700 dark:text-gray-300 
                           opacity-0 max-h-0 group-hover:max-h-40 group-hover:opacity-100 
                           overflow-hidden transition-all duration-300"
              >
                {feature.description}
              </p>
            </div>
          );
        })}
      </div>
    </section>
  );
}
