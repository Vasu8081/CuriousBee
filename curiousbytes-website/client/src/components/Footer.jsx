import React from "react";
import { FaInstagram, FaYoutube, FaLinkedin } from "react-icons/fa";

export default function Footer({
  brand = "Curious Bytes",
  tagline = "Stay curious. Keep progressing.",
  ctaText = "Experience Today",
  ctaHref = "/",
  contactLabel = "Contact Us",
  contactEmail = "curiousbytes67@gmail.com",
  privacyHref = "/privacy-policy",
  termsHref = "/terms-and-conditions",
  contactHref = "/contact",
}) {
  const year = new Date().getFullYear();

  return (
    <footer className="bg-white dark:bg-gray-900 text-gray-700 dark:text-gray-300 pt-16 pb-10 mt-24 border-t border-gray-200 dark:border-gray-700 font-sans">
      {/* Top: Brand + Contact */}
      <div className="max-w-7xl mx-auto px-4 flex flex-col md:flex-row md:items-start md:justify-between gap-8">
        {/* Brand */}
        <div>
          <h2 className="text-xl font-bold tracking-tight text-indigo-600 dark:text-indigo-400">
            {brand}
          </h2>
          <p className="text-sm mt-1 opacity-90">{tagline}</p>
        </div>

        {/* Contact Info */}
        <div className="text-left md:text-right space-y-3">
          <div className="text-sm opacity-90">
            <span className="font-semibold">{contactLabel}</span>
            <br />
            <a
              href={`mailto:${contactEmail}`}
              className="underline underline-offset-2 hover:no-underline break-words text-indigo-600 dark:text-indigo-400"
            >
              {contactEmail}
            </a>
          </div>
        </div>
      </div>

      {/* Divider */}
      <hr className="max-w-7xl mx-auto mt-12 mb-8 border-t border-gray-300 dark:border-gray-700" />

      {/* Bottom Bar */}
      <div className="max-w-7xl mx-auto px-4 flex flex-col sm:flex-row sm:items-center sm:justify-between gap-6 text-sm">
        {/* Copyright */}
        <p className="opacity-80">&copy; {year} {brand}. All rights reserved.</p>

        {/* Links */}
        <nav className="flex flex-wrap items-center gap-x-4 gap-y-2 justify-start sm:justify-center">
          <a
            href={privacyHref}
            className="hover:underline underline-offset-2 opacity-90 hover:opacity-100 transition"
          >
            Privacy Policy
          </a>
          <span className="opacity-50">&bull;</span>
          <a
            href={termsHref}
            className="hover:underline underline-offset-2 opacity-90 hover:opacity-100 transition"
          >
            Terms & Conditions
          </a>
          <span className="opacity-50">&bull;</span>
          <a
            href={contactHref}
            className="hover:underline underline-offset-2 opacity-90 hover:opacity-100 transition"
          >
            Contact Us
          </a>
        </nav>

        {/* Social Icons */}
        <div className="flex items-center gap-5 text-xl">
          <a
            href="https://instagram.com/_bhawwna"
            target="_blank"
            rel="noopener noreferrer"
            className="hover:scale-110 transition"
            aria-label="Instagram"
          >
            <FaInstagram className="text-pink-500" />
          </a>
          <a
            href="https://www.youtube.com/@curiousbytes67"
            target="_blank"
            rel="noopener noreferrer"
            className="hover:scale-110 transition"
            aria-label="YouTube"
          >
            <FaYoutube className="text-red-500" />
          </a>
          <a
            href="https://in.linkedin.com/in/bhawnachelani"
            target="_blank"
            rel="noopener noreferrer"
            className="hover:scale-110 transition"
            aria-label="LinkedIn"
          >
            <FaLinkedin className="text-sky-600" />
          </a>
        </div>
      </div>
    </footer>
  );
}
