import React from "react";
import { FaInstagram, FaYoutube, FaLinkedin } from "react-icons/fa";
import { brandColors } from "../constants/theme";

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
        <div>
          <h2
            className="text-xl font-bold tracking-tight"
            style={{ color: brandColors.primary.light }}
          >
            {brand}
          </h2>
          <p className="text-sm mt-1 opacity-90">{tagline}</p>
        </div>

        <div className="text-left md:text-right space-y-3">
          <div className="text-sm opacity-90">
            <span className="font-semibold">{contactLabel}</span>
            <br />
            <a
              href={`mailto:${contactEmail}`}
              className="underline underline-offset-2 hover:no-underline break-words"
              style={{ color: brandColors.primary.light }}
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
        <p className="opacity-80">
          &copy; {year} {brand}. All rights reserved.
        </p>

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

        <div className="flex items-center gap-5 text-xl">
          <a
            href="https://instagram.com/_bhawwna"
            target="_blank"
            rel="noopener noreferrer"
            className="hover:scale-110 transition"
            aria-label="Instagram"
          >
            <FaInstagram style={{ color: brandColors.instagram }} />
          </a>
          <a
            href="https://www.youtube.com/@curiousbytes67"
            target="_blank"
            rel="noopener noreferrer"
            className="hover:scale-110 transition"
            aria-label="YouTube"
          >
            <FaYoutube style={{ color: brandColors.youtube }} />
          </a>
          <a
            href="https://in.linkedin.com/in/bhawnachelani"
            target="_blank"
            rel="noopener noreferrer"
            className="hover:scale-110 transition"
            aria-label="LinkedIn"
          >
            <FaLinkedin style={{ color: brandColors.linkedin }} />
          </a>
        </div>
      </div>
    </footer>
  );
}
