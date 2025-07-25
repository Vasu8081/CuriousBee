FROM node:18

WORKDIR /app

# Copy package files (even if unused, needed for lockfile format)
COPY package.json package-lock.json* ./

# Install all required dependencies directly here (dev + prod)
RUN npm install \
    react \
    react-dom \
    react-router-dom \
    vite \
    @vitejs/plugin-react \
    @tailwindcss/aspect-ratio \
    lucide-react \
    react-icons \
    tailwindcss \
    postcss \
    autoprefixer \
    serve

# Copy full source code (after npm install to avoid overwrite issues)
COPY . .

ENV NODE_ENV=development

# Default: LIVE_MODE=1 means hot reload; otherwise production
CMD ["sh", "-c", "\
  if [ \"$LIVE_MODE\" = \"1\" ]; then \
    echo '[INFO] Running in LIVE mode (dev server with hot reload)'; \
    npx vite --host; \
  else \
    echo '[INFO] Running in STABLE mode (production build)'; \
    npx vite build && npx serve -s dist -l 5173; \
  fi"]