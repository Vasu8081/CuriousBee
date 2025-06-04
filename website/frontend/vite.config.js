import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";

export default defineConfig({
  plugins: [react()],
  server: {
    host: true,
    port: 5173,
    allowedHosts: ["www.curiousbytes.in", "curiousbytes.in"],
    proxy: {
      "/api": "http://curiousbytes-backend:8000",
    },
  },
});