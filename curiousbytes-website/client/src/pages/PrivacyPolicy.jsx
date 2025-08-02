import React from "react";
import { semanticColors, brandColors } from "../constants/theme"; // adjust path if needed

export default function PrivacyPolicy() {
  return (
    <div className="max-w-3xl mx-auto px-4 py-16 text-gray-900 dark:text-white font-sans">
      <h1 className="text-4xl font-bold mb-8 text-center">Privacy Policy</h1>
      <p className="mb-6 text-gray-600 dark:text-gray-300 text-center">
        We value your privacy. This page outlines what data we collect and how we use it.
      </p>

      <div className="space-y-8">
        {[
          {
            title: "1. Information We Collect",
            content: (
              <ul className="list-disc ml-6 space-y-2 text-gray-700 dark:text-gray-300">
                <li>Personal info like name and email when submitted via forms.</li>
                <li>Device and browser data (via analytics tools).</li>
                <li>Voluntary inputs like comments or messages.</li>
              </ul>
            ),
          },
          {
            title: "2. How We Use Your Data",
            content: (
              <ul className="list-disc ml-6 space-y-2 text-gray-700 dark:text-gray-300">
                <li>To respond to queries and provide support.</li>
                <li>To improve content and website experience.</li>
                <li>To personalize your interactions with us.</li>
              </ul>
            ),
          },
          {
            title: "3. Cookies",
            content: (
              <p className="text-gray-700 dark:text-gray-300">
                We may use cookies for analytics or user preferences. You can control cookies via your browser settings.
              </p>
            ),
          },
          {
            title: "4. Third-party Services",
            content: (
              <p className="text-gray-700 dark:text-gray-300">
                Our site may include content from platforms like YouTube, Google Analytics, Instagram, and LinkedIn.
                These services may collect anonymous usage data.
              </p>
            ),
          },
          {
            title: "5. Changes",
            content: (
              <p className="text-gray-700 dark:text-gray-300">
                We may update this policy. Revisions will be reflected on this page with an updated date.
              </p>
            ),
          },
          {
            title: "6. Contact",
            content: (
              <p className="text-gray-700 dark:text-gray-300">
                If you have questions about this policy, contact us at{" "}
                <a
                  href="mailto:curiousbytes67@gmail.com"
                  className="underline text-blue-600 dark:text-blue-400 hover:text-blue-800"
                >
                  curiousbytes67@gmail.com
                </a>.
              </p>
            ),
          },
        ].map((section, index) => (
          <section
            key={index}
            className="transition-all hover:scale-[1.01] hover:bg-gray-100 dark:hover:bg-gray-800 p-4 rounded-lg"
          >
            <h2 className="text-xl font-semibold mb-2 text-indigo-600 dark:text-indigo-400">
              {section.title}
            </h2>
            {section.content}
          </section>
        ))}
      </div>
    </div>
  );
}
