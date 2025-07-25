FROM node:18

WORKDIR /app

# Install all dependencies directly via npm
RUN npm install react react-dom react-router-dom \
    vite @vitejs/plugin-react @tailwindcss/aspect-ratio \
    lucide-react react-icons tailwindcss postcss autoprefixer \
    serve

# Copy the entire app (excluding node_modules via .dockerignore)
COPY . .

ENV NODE_ENV=development

# Use LIVE_MODE to toggle behavior
CMD ["sh", "-c", "\
  if [ \"$LIVE_MODE\" = \"1\" ]; then \
    echo '[INFO] Running in LIVE mode (dev server with hot reload)'; \
    npx vite --host; \
  else \
    echo '[INFO] Running in STABLE mode (production build)'; \
    npx vite build && npx serve -s dist -l 5173; \
  fi"]