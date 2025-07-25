FROM node:18

WORKDIR /app

# Copy package files and install all dependencies
COPY package.json package-lock.json* ./
RUN npm install

# Install dev dependencies that might not be in package.json
RUN npm install \
    @vitejs/plugin-react \
    @tailwindcss/aspect-ratio \
    lucide-react \
    react-icons

# Copy rest of the client code
COPY . .

# Default to development (live mode) unless overridden
ENV NODE_ENV=development

# Entry point with conditional mode
CMD ["sh", "-c", "\
  if [ \"$LIVE_MODE\" = \"1\" ]; then \
    echo '[INFO] Running in LIVE mode (dev server with hot reload)'; \
    npx vite --host; \
  else \
    echo '[INFO] Running in STABLE mode (production build)'; \
    npm run build && npx serve -s dist -l 5173; \
  fi"]