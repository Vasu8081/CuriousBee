FROM node:18

WORKDIR /app

# Copy and install all dependencies including dev
COPY package.json package-lock.json* ./
RUN npm install

# Copy the rest of the source
COPY . .

# Default to development
ENV NODE_ENV=development

# LIVE_MODE=1 → hot reload (live), otherwise → stable mode
CMD ["sh", "-c", "\
  if [ \"$LIVE_MODE\" = \"1\" ]; then \
    echo '[INFO] Running in LIVE mode (dev server with hot reload)'; \
    npx vite --host; \
  else \
    echo '[INFO] Running in STABLE mode (production build)'; \
    npm run build && npx serve -s dist -l 5173; \
  fi"]