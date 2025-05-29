#!/bin/sh

echo "🔧 Starting BeeTracker Server..."

. /app/config.cfg

export NGROK_AUTHTOKEN="${NGROK_AUTHTOKEN:-$CFG_NGROK_AUTHTOKEN}"
export JWT_SECRET="${JWT_SECRET:-$CFG_JWT_SECRET}"
export JWT_REFRESH_SECRET="${JWT_REFRESH_SECRET:-$CFG_JWT_REFRESH_SECRET}"
export CLIENT_ID="${CLIENT_ID:-$CFG_CLIENT_ID}"
export GCS_CREDENTIALS_PATH="${GCS_CREDENTIALS_PATH:-$CFG_GCS_CREDENTIALS_PATH}"
export BUCKET_NAME="${BUCKET_NAME:-$CFG_BUCKET_NAME}"
export APPLE_P8_CERTIFICATE_PATH="${APPLE_P8_CERTIFICATE_PATH:-$CFG_APPLE_P8_CERTIFICATE_PATH}"
export APPLE_TEAM_ID="${APPLE_TEAM_ID:-$CFG_APPLE_TEAM_ID}"
export APPLE_KEY_ID="${APPLE_KEY_ID:-$CFG_APPLE_KEY_ID}"
export APPLE_BUNDLE_ID="${APPLE_BUNDLE_ID:-$CFG_APPLE_BUNDLE_ID}"
export LOCAL_SERVER="${LOCAL_SERVER:-$CFG_LOCAL_SERVER}"
export ENABLE_NGROK="${ENABLE_NGROK:-$CFG_ENABLE_NGROK}"
export NGROK_PORT="${NGROK_PORT:-$CFG_NGROK_PORT}"
export USE_SANDBOX="${USE_SANDBOX:-$CFG_USE_SANDBOX}"
export REFRESH_TOKEN_EXPIRY="${REFRESH_TOKEN_EXPIRY:-$CFG_REFRESH_TOKEN_EXPIRY}"
export ACCESS_TOKEN_EXPIRY="${ACCESS_TOKEN_EXPIRY:-$CFG_ACCESS_TOKEN_EXPIRY}"

echo "🔍 Environment variables set:"
echo "  - NGROK_AUTHTOKEN: ${NGROK_AUTHTOKEN:-not set}"
echo "  - JWT_SECRET: ${JWT_SECRET:-not set}"
echo "  - JWT_REFRESH_SECRET: ${JWT_REFRESH_SECRET:-not set}"
echo "  - CLIENT_ID: ${CLIENT_ID:-not set}"
echo "  - GCS_CREDENTIALS_PATH: ${GCS_CREDENTIALS_PATH:-not set}"
echo "  - BUCKET_NAME: ${BUCKET_NAME:-not set}"
echo "  - APPLE_P8_CERTIFICATE_PATH: ${APPLE_P8_CERTIFICATE_PATH:-not set}"
echo "  - APPLE_TEAM_ID: ${APPLE_TEAM_ID:-not set}"
echo "  - APPLE_KEY_ID: ${APPLE_KEY_ID:-not set}"
echo "  - APPLE_BUNDLE_ID: ${APPLE_BUNDLE_ID:-not set}"
echo "  - LOCAL_SERVER: ${LOCAL_SERVER:-not set}"
echo "  - ENABLE_NGROK: ${ENABLE_NGROK:-not set}"
echo "  - NGROK_PORT: ${NGROK_PORT:-not set}"
echo "  - USE_SANDBOX: ${USE_SANDBOX:-not set}"
echo "  - REFRESH_TOKEN_EXPIRY: ${REFRESH_TOKEN_EXPIRY:-not set}"
echo "  - ACCESS_TOKEN_EXPIRY: ${ACCESS_TOKEN_EXPIRY:-not set}"

uvicorn app.main:app --reload --host 0.0.0.0 --port 8000
echo "🚀 Starting BeeTracker Server with Uvicorn..."

# Prevent container from exiting
echo "✅ Setup complete. Container will keep running."
tail -f /dev/null