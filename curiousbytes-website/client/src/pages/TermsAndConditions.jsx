import React from "react";

export default function TermsAndConditions() {
  return (
    <div className="max-w-3xl mx-auto px-4 py-16 text-gray-900 dark:text-white font-sans">
      <h1 className="text-4xl font-bold mb-8 text-center">Terms and Conditions</h1>
      <p className="mb-6 text-gray-700 dark:text-gray-300 text-center">
        By using this website, you agree to the following terms and conditions. Please read them carefully.
      </p>

      <div className="space-y-8">
        <section className="transition-all hover:scale-[1.01] hover:bg-gray-100 dark:hover:bg-gray-800 p-4 rounded-lg">
          <h2 className="text-xl font-semibold mb-2 text-indigo-600 dark:text-indigo-400">1. Use of Content</h2>
          <p className="text-gray-700 dark:text-gray-300">
            All content on this site is owned by or licensed to Bhawna Chelani. You may not reuse, reproduce, or
            redistribute content without explicit permission.
          </p>
        </section>

        <section className="transition-all hover:scale-[1.01] hover:bg-gray-100 dark:hover:bg-gray-800 p-4 rounded-lg">
          <h2 className="text-xl font-semibold mb-2 text-indigo-600 dark:text-indigo-400">2. No Guarantees</h2>
          <p className="text-gray-700 dark:text-gray-300">
            While we strive for accuracy, we do not guarantee results. Content is educational and does not constitute
            professional or legal advice.
          </p>
        </section>

        <section className="transition-all hover:scale-[1.01] hover:bg-gray-100 dark:hover:bg-gray-800 p-4 rounded-lg">
          <h2 className="text-xl font-semibold mb-2 text-indigo-600 dark:text-indigo-400">3. External Links</h2>
          <p className="text-gray-700 dark:text-gray-300">
            This site may link to third-party platforms (e.g., YouTube, Instagram). We are not responsible for their
            content or privacy practices.
          </p>
        </section>

        <section className="transition-all hover:scale-[1.01] hover:bg-gray-100 dark:hover:bg-gray-800 p-4 rounded-lg">
          <h2 className="text-xl font-semibold mb-2 text-indigo-600 dark:text-indigo-400">4. Limitation of Liability</h2>
          <p className="text-gray-700 dark:text-gray-300">
            We are not liable for any damages resulting from the use of this site or its content.
          </p>
        </section>

        <section className="transition-all hover:scale-[1.01] hover:bg-gray-100 dark:hover:bg-gray-800 p-4 rounded-lg">
          <h2 className="text-xl font-semibold mb-2 text-indigo-600 dark:text-indigo-400">5. Changes to Terms</h2>
          <p className="text-gray-700 dark:text-gray-300">
            These terms may be updated from time to time. Updates will be posted on this page.
          </p>
        </section>

        <section className="transition-all hover:scale-[1.01] hover:bg-gray-100 dark:hover:bg-gray-800 p-4 rounded-lg">
          <h2 className="text-xl font-semibold mb-2 text-indigo-600 dark:text-indigo-400">6. Contact</h2>
          <p className="text-gray-700 dark:text-gray-300">
            For any questions about these terms, please contact{" "}
            <a href="mailto:curiousbytes67@gmail.com" className="underline text-blue-600 dark:text-blue-400 hover:text-blue-800">
              curiousbytes67@gmail.com
            </a>.
          </p>
        </section>
      </div>
    </div>
  );
}
