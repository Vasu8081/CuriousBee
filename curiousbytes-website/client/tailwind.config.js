import aspectRatio from '@tailwindcss/aspect-ratio';

module.exports = {
  darkMode: 'class', // 'media' is automatic, 'class' is manual toggle
  content: ["./index.html", "./src/**/*.{js,ts,jsx,tsx}"],
  theme: {
    extend: {
      fontFamily: {
        inter: ['Inter', 'sans-serif'],
        poppins: ['Poppins', 'sans-serif'],
        rubik: ['Rubik', 'sans-serif'],
      },
    },
  },
  plugins: [aspectRatio],
};