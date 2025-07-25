FROM node:18

WORKDIR /app

# Copy only package files first and install dependencies
COPY package.json package-lock.json* ./
RUN npm install

# Now copy all source files, but .dockerignore prevents overwriting node_modules
COPY . .

ENV NODE_ENV=development

CMD ["sh", "-c", "\
  if [ \"$LIVE_MODE\" = \"1\" ]; then \
    echo '[INFO] Running in LIVE mode (dev server with hot reload)'; \
    npx vite --host; \
  else \
    echo '[INFO] Running in STABLE mode (production build)'; \
    npm run build && npx serve -s dist -l 5173; \
  fi"]