import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";

const isLive = process.env.LIVE_MODE === "1";

export default defineConfig({
  plugins: [react()],
  server: {
    host: true,
    port: 5173,
    allowedHosts: ["www.curiousbytes.in", "curiousbytes.in", "live.curiousbytes.in"],
    proxy: {
      "/api": isLive
        ? "http://curiousbytes-backend-live:8000"
        : "http://curiousbytes-backend-stable:8001",
    },
  },
});