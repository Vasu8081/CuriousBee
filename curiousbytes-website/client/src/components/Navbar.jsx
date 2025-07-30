import React, { useState } from "react";
import { Link } from "react-router-dom";
import { Menu, X } from "lucide-react";
import ThemeToggle from "./ThemeToggle";

const navItems = [
  { name: "Home", path: "/" },
  { name: "Videos", path: "/videos" },
  { name: "Resources", path: "/resources" },
  { name: "Blog", path: "/blog" },
];

export default function Navbar() {
  const [isOpen, setIsOpen] = useState(false);

  return (
    <header className="sticky top-0 z-50 backdrop-blur bg-white/80 dark:bg-gray-900/80 shadow-sm transition-all">
      <div className="max-w-7xl mx-auto px-4 py-3 flex items-center justify-between">
        {/* Branding */}
        <Link to="/" className="text-2xl font-bold tracking-tight text-indigo-600 dark:text-indigo-400">
          Curious Bytes
        </Link>

        {/* Desktop Nav */}
        <nav className="hidden md:flex items-center space-x-6">
          {navItems.map((item) => (
            <Link
              key={item.name}
              to={item.path}
              className="text-gray-700 dark:text-gray-100 hover:text-indigo-600 dark:hover:text-indigo-400 font-medium transition"
            >
              {item.name}
            </Link>
          ))}
          <Link
            to="/videos"
            className="ml-4 bg-indigo-500 hover:bg-indigo-600 text-white px-4 py-2 rounded-full font-semibold transition"
          >
            Get Started
          </Link>
          <ThemeToggle />
        </nav>

        {/* Mobile Toggle */}
        <div className="md:hidden flex items-center space-x-2">
          <ThemeToggle />
          <button
            onClick={() => setIsOpen(!isOpen)}
            className="p-2 rounded text-indigo-600 dark:text-white hover:bg-indigo-100 dark:hover:bg-gray-700"
          >
            {isOpen ? <X size={24} /> : <Menu size={24} />}
          </button>
        </div>
      </div>

      {/* Mobile Menu */}
      {isOpen && (
        <div className="md:hidden px-4 pb-4 space-y-2 bg-white dark:bg-gray-900">
          {navItems.map((item) => (
            <Link
              key={item.name}
              to={item.path}
              className="block text-gray-700 dark:text-gray-100 hover:text-indigo-600 dark:hover:text-indigo-400 font-medium"
              onClick={() => setIsOpen(false)}
            >
              {item.name}
            </Link>
          ))}
          <Link
            to="/videos"
            className="block mt-2 bg-indigo-500 hover:bg-indigo-600 text-white text-center px-4 py-2 rounded-full font-semibold"
            onClick={() => setIsOpen(false)}
          >
            Get Started
          </Link>
        </div>
      )}
    </header>
  );
}
