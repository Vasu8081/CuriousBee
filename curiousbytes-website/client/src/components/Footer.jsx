// src/components/Footer.jsx
import React from "react";
import { FaInstagram, FaYoutube, FaLinkedin } from "react-icons/fa";

export default function Footer({
  brand = "CuriousBytes",
  tagline = "Stay curious. Keep progressing.",
  ctaText = "Experience Today",
  ctaHref = "/",
  contactLabel = "Contact us",
  contactEmail = "curiousbytes67@gmail.com",
  privacyHref = "/privacy",
  termsHref = "/terms",
  contactHref = "/contact",
}) {
  const year = new Date().getFullYear();

  return (
    <footer className="bg-black text-white pt-16 pb-10 mt-24">
      {/* Top: Brand + CTA */}
      <div className="max-w-7xl mx-auto px-4 flex flex-col md:flex-row md:items-start md:justify-between gap-8">
        {/* Left: Brand */}
        <div>
          <h2 className="text-xl font-bold tracking-tight">{brand}</h2>
          <p className="text-sm mt-1 opacity-90">{tagline}</p>
        </div>

        {/* Right: CTA + Contact */}
        <div className="text-left md:text-right space-y-3">

          <div className="text-sm opacity-90">
            <span className="font-semibold">{contactLabel}</span>
            <br />
            <a
              href={`mailto:${contactEmail}`}
              className="underline underline-offset-2 hover:no-underline break-words"
            >
              {contactEmail}
            </a>
          </div>
        </div>
      </div>

      {/* Divider */}
      <hr className="max-w-7xl mx-auto mt-12 mb-8 border-t border-white/30" />

      {/* Bottom bar */}
      <div className="max-w-7xl mx-auto px-4 flex flex-col sm:flex-row sm:items-center sm:justify-between gap-6 text-sm">
        {/* Left: Copyright */}
        <p className="opacity-90">
          &copy; {year} {brand}. All rights reserved.
        </p>

        {/* Middle: Legal links */}
        <nav className="flex flex-wrap items-center gap-x-4 gap-y-2 justify-start sm:justify-center">
          <a
            href={privacyHref}
            className="hover:underline underline-offset-2 opacity-90 hover:opacity-100"
          >
            Privacy Policy
          </a>
          <span aria-hidden="true" className="opacity-50">
            &bull;
          </span>
          <a
            href={termsHref}
            className="hover:underline underline-offset-2 opacity-90 hover:opacity-100"
          >
            Terms &amp; Conditions
          </a>
          <span aria-hidden="true" className="opacity-50">
            &bull;
          </span>
          <a
            href={contactHref}
            className="hover:underline underline-offset-2 opacity-90 hover:opacity-100"
          >
            Contact Us
          </a>
        </nav>

        {/* Right: Social icons */}
        <div className="flex items-center justify-start sm:justify-end gap-5 text-xl">
          <a
            href="https://instagram.com/_bhawwna"
            target="_blank"
            rel="noopener noreferrer"
            aria-label="Instagram"
            className="hover:opacity-80 focus-visible:outline focus-visible:outline-2 focus-visible:outline-offset-2 focus-visible:outline-white"
          >
            <FaInstagram />
          </a>
          <a
            href="https://www.youtube.com/@curiousbytes67"
            target="_blank"
            rel="noopener noreferrer"
            aria-label="YouTube"
            className="hover:opacity-80 focus-visible:outline focus-visible:outline-2 focus-visible:outline-offset-2 focus-visible:outline-white"
          >
            <FaYoutube />
          </a>
          <a
            href="https://in.linkedin.com/in/bhawnachelani"
            target="_blank"
            rel="noopener noreferrer"
            aria-label="LinkedIn"
            className="hover:opacity-80 focus-visible:outline focus-visible:outline-2 focus-visible:outline-offset-2 focus-visible:outline-white"
          >
            <FaLinkedin />
          </a>
        </div>
      </div>
    </footer>
  );
}