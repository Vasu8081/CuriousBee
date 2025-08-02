import React, { useState } from "react";
import { Link, useLocation } from "react-router-dom";
import { Menu, X } from "lucide-react";
import ThemeToggle from "./ThemeToggle";
import { brandColors } from "../constants/theme";

const navItems = [
  { name: "Home", path: "/" },
  { name: "Videos", path: "/videos" },
  { name: "Resources", path: "/resources" },
  { name: "Blog", path: "/blog" },
];

export default function Navbar() {
  const [isOpen, setIsOpen] = useState(false);
  const location = useLocation();
  const currentPath = location.pathname;

  return (
    <header className="sticky top-0 z-50 backdrop-blur bg-white/80 dark:bg-gray-900/80 shadow-sm transition-all">
      <div className="max-w-7xl mx-auto px-4 py-3 flex items-center justify-between">
        {/* Branding */}
        <Link
          to="/"
          className="text-2xl font-bold tracking-tight"
          style={{ color: brandColors.primary.light }}
        >
          Curious Bytes
        </Link>

        {/* Desktop Navigation */}
        <nav className="hidden md:flex items-center space-x-6">
          {navItems.map((item) => (
            <Link
              key={item.name}
              to={item.path}
              className={`font-medium transition hover:underline underline-offset-4 ${
                currentPath === item.path
                  ? "text-indigo-600 dark:text-indigo-400"
                  : "text-gray-700 dark:text-gray-100 hover:text-indigo-600 dark:hover:text-indigo-400"
              }`}
            >
              {item.name}
            </Link>
          ))}

          <Link
            to="/videos"
            className="ml-4 px-4 py-2 rounded-full font-semibold text-white transition"
            style={{
              backgroundColor: brandColors.primary.light,
            }}
            onMouseOver={(e) => {
              e.currentTarget.style.backgroundColor = brandColors.hover.gate;
            }}
            onMouseOut={(e) => {
              e.currentTarget.style.backgroundColor = brandColors.primary.light;
            }}
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
            className="p-2 rounded hover:bg-indigo-100 dark:hover:bg-gray-700 transition"
            style={{ color: brandColors.primary.light }}
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
              className={`block font-medium hover:underline underline-offset-4 ${
                currentPath === item.path
                  ? "text-indigo-600 dark:text-indigo-400"
                  : "text-gray-700 dark:text-gray-100 hover:text-indigo-600 dark:hover:text-indigo-400"
              }`}
              onClick={() => setIsOpen(false)}
            >
              {item.name}
            </Link>
          ))}

          <Link
            to="/videos"
            className="block mt-2 text-center px-4 py-2 rounded-full font-semibold text-white transition"
            style={{
              backgroundColor: brandColors.primary.light,
            }}
            onMouseOver={(e) => {
              e.currentTarget.style.backgroundColor = brandColors.hover.gate;
            }}
            onMouseOut={(e) => {
              e.currentTarget.style.backgroundColor = brandColors.primary.light;
            }}
            onClick={() => setIsOpen(false)}
          >
            Get Started
          </Link>
        </div>
      )}
    </header>
  );
}
