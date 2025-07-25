# 🐍 Base image with Python 3.11
FROM python:3.11-slim

# Set working directory
WORKDIR /app

# Install system dependencies (PostgreSQL build requirements)
RUN apt-get update && apt-get install -y \
    gcc \
    libpq-dev \
 && rm -rf /var/lib/apt/lists/*

# Copy application code
COPY . .

# Install Python dependencies
RUN pip install --no-cache-dir --upgrade pip \
 && pip install --no-cache-dir -r requirements.txt

# Start FastAPI app with or without --reload based on LIVE_MODE
CMD ["sh", "-c", "\
  alembic -c alembic.ini upgrade head && \
  if [ \"$LIVE_MODE\" = \"1\" ]; then \
    uvicorn app.main:app --reload --host 0.0.0.0 --port 8000; \
  else \
    uvicorn app.main:app --host 0.0.0.0 --port 8000; \
  fi"]