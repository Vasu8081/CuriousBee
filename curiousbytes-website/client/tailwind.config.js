/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  darkMode: 'class', // Enables manual dark mode via class
  theme: {
    extend: {
      colors: {
        primary: '#FF5722', // A modern orange
        accent: '#FCD34D',   // Soft yellow
        bgDark: '#121212',
        bgLight: '#f9fafb',
      },
      fontFamily: {
        sans: ['Inter', 'ui-sans-serif', 'system-ui'],
      },
    },
  },
  plugins: [],
};