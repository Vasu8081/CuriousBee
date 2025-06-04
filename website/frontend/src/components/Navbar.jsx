import React, { useState } from "react";
import { Link } from "react-router-dom";
import { Menu, X } from "lucide-react";
import ThemeToggle from "./ThemeToggle";

const navItems = [
  { name: "Home", path: "/" },
  { name: "Videos", path: "/videos" },
  { name: "Key Points", path: "/keypoints" },  // 🔥 Added this line
  { name: "GATE Prep", path: "/gate" },
  { name: "Interviews", path: "/interviews" },
  { name: "Blog", path: "/blog" },
];

export default function Navbar() {
  const [isOpen, setIsOpen] = useState(false);

  return (
    <header className="sticky top-0 z-50 backdrop-blur bg-white/80 dark:bg-gray-900/80 shadow-md transition-all">
      <div className="max-w-7xl mx-auto px-4 py-3 flex items-center justify-between">
        <Link to="/" className="flex items-center space-x-2">
          <img
            src="/logo-dark.png"
            alt="Curious Bytes"
            className="h-12 sm:h-14 w-auto dark:hidden"
          />
          <img
            src="/logo-white.png"
            alt="Curious Bytes"
            className="h-12 sm:h-14 w-auto hidden dark:block"
          />
          <span className="text-2xl font-bold text-indigo-600 dark:text-white ml-2">
            CuriousBytes
          </span>
        </Link>

        <div className="md:hidden flex items-center space-x-2">
          <ThemeToggle />
          <button
            onClick={() => setIsOpen(!isOpen)}
            className="p-2 rounded text-indigo-600 dark:text-white hover:bg-indigo-100 dark:hover:bg-gray-700"
          >
            {isOpen ? <X size={24} /> : <Menu size={24} />}
          </button>
        </div>

        <nav className="hidden md:flex items-center space-x-6">
          {navItems.map((item) => (
            <Link
              key={item.name}
              to={item.path}
              className="text-gray-700 dark:text-white hover:text-indigo-600 dark:hover:text-indigo-400 font-medium"
            >
              {item.name}
            </Link>
          ))}
          <ThemeToggle />
        </nav>
      </div>

      {isOpen && (
        <div className="md:hidden px-4 pb-4 space-y-2">
          {navItems.map((item) => (
            <Link
              key={item.name}
              to={item.path}
              className="block text-gray-700 dark:text-white hover:text-indigo-600 dark:hover:text-indigo-400 font-medium"
              onClick={() => setIsOpen(false)}
            >
              {item.name}
            </Link>
          ))}
        </div>
      )}
    </header>
  );
}