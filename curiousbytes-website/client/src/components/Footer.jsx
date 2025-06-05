// src/components/Footer.jsx
import React from 'react';
import { FaInstagram, FaYoutube, FaLinkedin } from "react-icons/fa";

export default function Footer() {
  return (
    <footer className="bg-black text-gray-300 py-8 border-t border-gray-800">
      <div className="max-w-4xl mx-auto text-center space-y-4">
        <p className="text-sm">
          &copy; {new Date().getFullYear()}{" "}
          <span className="font-semibold text-white">CuriousBytes</span>
        </p>

        <p className="text-sm font-semibold text-yellow-400">
          Stay curious and keep progressing
        </p>

        <div className="flex justify-center space-x-6 text-lg">
          <a
            href="https://instagram.com/_bhawwna"
            target="_blank"
            rel="noopener noreferrer"
            className="text-pink-500 hover:text-pink-400 transition-colors"
            title="Instagram"
          >
            <FaInstagram />
          </a>
          <a
            href="https://www.youtube.com/@curiousbytes67"
            target="_blank"
            rel="noopener noreferrer"
            className="text-red-500 hover:text-red-400 transition-colors"
            title="YouTube"
          >
            <FaYoutube />
          </a>
          <a
            href="https://in.linkedin.com/in/bhawnachelani"
            target="_blank"
            rel="noopener noreferrer"
            className="text-blue-500 hover:text-blue-400 transition-colors"
            title="LinkedIn"
          >
            <FaLinkedin />
          </a>
        </div>
      </div>
    </footer>
  );
}