import React from "react";
import { Link } from "react-router-dom";
import InstructorIntro from "../components/InstructorIntro";
import ConnectWithUs from "../components/ConnectWithUs";
import WhyCuriousBytes from "../components/WhyCuriousBytes";
import HeroSection from "../components/HeroSection";

export default function Home() {
  return (
    <div className="bg-white dark:bg-gray-900 text-black dark:text-white">
      {/* Hero Section */}
      <HeroSection />
      
      {/* Why Curious Bytes */}
      <WhyCuriousBytes />

      {/* Instructor Intro + Socials */}
      <InstructorIntro />
      <ConnectWithUs />
    </div>
  );
}
