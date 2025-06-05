FROM node:18

WORKDIR /app

COPY package.json package-lock.json* ./

# Install Tailwind plugin + dependencies
RUN npm install \
    @vitejs/plugin-react \
    @tailwindcss/aspect-ratio \
    lucide-react \
    react-icons

COPY . .
ENV NODE_ENV=development
CMD ["npx", "vite", "--host"]