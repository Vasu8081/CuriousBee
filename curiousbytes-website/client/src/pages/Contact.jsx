import React from "react";
import { FaInstagram, FaLinkedin } from "react-icons/fa";

export default function ContactPage() {
  return (
    <div className="max-w-3xl mx-auto px-4 py-16 text-gray-900 dark:text-white font-sans">
      <h1 className="text-4xl font-bold mb-6 text-center">Get in Touch</h1>
      <p className="mb-8 text-gray-600 dark:text-gray-300 text-center">
        Questions, feedback, or collaboration ideas? We'd love to hear from you!
      </p>

      <form className="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6 space-y-6 transition-all">
        <div className="group">
          <label htmlFor="name" className="block font-medium text-gray-800 dark:text-gray-200 mb-1">
            Name
          </label>
          <input
            type="text"
            id="name"
            name="name"
            className="w-full bg-gray-100 dark:bg-gray-900 border border-gray-300 dark:border-gray-600 rounded px-4 py-2 text-gray-900 dark:text-white focus:outline-none focus:ring-2 focus:ring-blue-500 transition-all"
            placeholder="Your name"
          />
        </div>

        <div className="group">
          <label htmlFor="email" className="block font-medium text-gray-800 dark:text-gray-200 mb-1">
            Email
          </label>
          <input
            type="email"
            id="email"
            name="email"
            className="w-full bg-gray-100 dark:bg-gray-900 border border-gray-300 dark:border-gray-600 rounded px-4 py-2 text-gray-900 dark:text-white focus:outline-none focus:ring-2 focus:ring-blue-500 transition-all"
            placeholder="your@email.com"
          />
        </div>

        <div className="group">
          <label htmlFor="message" className="block font-medium text-gray-800 dark:text-gray-200 mb-1">
            Message
          </label>
          <textarea
            id="message"
            name="message"
            rows="5"
            className="w-full bg-gray-100 dark:bg-gray-900 border border-gray-300 dark:border-gray-600 rounded px-4 py-2 text-gray-900 dark:text-white focus:outline-none focus:ring-2 focus:ring-blue-500 transition-all"
            placeholder="Write your message here..."
          ></textarea>
        </div>

        <button
          type="submit"
          className="bg-blue-600 hover:bg-blue-700 text-white font-medium px-6 py-2 rounded-lg transition-transform transform hover:scale-105 duration-300"
        >
          Send Message
        </button>
      </form>

      <p className="text-sm text-gray-600 dark:text-gray-400 mt-8 text-center">
        Or email us directly at{" "}
        <a href="mailto:curiousbytes67@gmail.com" className="underline text-blue-600 dark:text-blue-400 hover:text-blue-800">
          curiousbytes67@gmail.com
        </a>
      </p>

      <div className="mt-8 text-center">
        <p className="mb-3 text-gray-800 dark:text-gray-300">You can also find us on:</p>
        <div className="flex justify-center gap-8 text-2xl">
          <a
            href="https://instagram.com/_bhawwna"
            target="_blank"
            rel="noopener noreferrer"
            className="hover:text-pink-500 transition-transform transform hover:scale-110"
            aria-label="Instagram"
          >
            <FaInstagram />
          </a>
          <a
            href="https://in.linkedin.com/in/bhawnachelani"
            target="_blank"
            rel="noopener noreferrer"
            className="hover:text-sky-500 transition-transform transform hover:scale-110"
            aria-label="LinkedIn"
          >
            <FaLinkedin />
          </a>
        </div>
      </div>
    </div>
  );
}
