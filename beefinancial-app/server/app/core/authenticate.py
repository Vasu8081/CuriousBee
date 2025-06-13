import os
import logging
import jwt
import datetime
import re
from fastapi import Request, HTTPException, status, Depends
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials

# Load secrets from environment
JWT_ACCESS_TOKEN_SECRET_KEY = os.environ.get("JWT_ACCESS_TOKEN_SECRET_KEY", "your-access-token-secret")
JWT_REFRESH_TOKEN_SECRET_KEY = os.environ.get("JWT_REFRESH_TOKEN_SECRET_KEY", "your-refresh-token-secret")
JWT_ALGORITHM = os.environ.get("JWT_ALGORITHM", "HS256")

# Parse durations like "30d", "1h"
def parse_duration(value):
    match = re.match(r"(\\d+)([dhms])", value.strip())
    if not match:
        return None
    num, unit = int(match.group(1)), match.group(2)
    if unit == "d":
        return datetime.timedelta(days=num)
    elif unit == "h":
        return datetime.timedelta(hours=num)
    elif unit == "m":
        return datetime.timedelta(minutes=num)
    elif unit == "s":
        return datetime.timedelta(seconds=num)
    return None

JWT_ACCESS_TOKEN_EXPIRATION_TIME = parse_duration(os.environ.get("JWT_ACCESS_TOKEN_EXPIRATION_TIME", "15m")) or datetime.timedelta(minutes=15)
JWT_REFRESH_TOKEN_EXPIRATION_TIME = parse_duration(os.environ.get("JWT_REFRESH_TOKEN_EXPIRATION_TIME", "30d")) or datetime.timedelta(days=30)

security = HTTPBearer()  # FastAPI’s way to extract the Bearer token

class Authenticate:
    def __init__(self):
        logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(message)s")

    def generate_tokens(self, email: str):
        now = datetime.datetime.utcnow()

        access_payload = {
            "sub": email,
            "type": "access",
            "exp": now + JWT_ACCESS_TOKEN_EXPIRATION_TIME
        }
        access_token = jwt.encode(access_payload, JWT_ACCESS_TOKEN_SECRET_KEY, algorithm=JWT_ALGORITHM)

        refresh_payload = {
            "sub": email,
            "type": "refresh",
            "exp": now + JWT_REFRESH_TOKEN_EXPIRATION_TIME
        }
        refresh_token = jwt.encode(refresh_payload, JWT_REFRESH_TOKEN_SECRET_KEY, algorithm=JWT_ALGORITHM)

        return {
            "access_token": access_token if isinstance(access_token, str) else access_token.decode("utf-8"),
            "refresh_token": refresh_token if isinstance(refresh_token, str) else refresh_token.decode("utf-8")
        }

    def decode_refresh_token(self, token: str):
        try:
            return jwt.decode(token, JWT_REFRESH_TOKEN_SECRET_KEY, algorithms=[JWT_ALGORITHM])
        except jwt.ExpiredSignatureError:
            raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Token expired")
        except jwt.InvalidTokenError:
            raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Invalid token")

    def decode_access_token(self, token: str):
        try:
            return jwt.decode(token, JWT_ACCESS_TOKEN_SECRET_KEY, algorithms=[JWT_ALGORITHM])
        except jwt.ExpiredSignatureError:
            raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Token expired")
        except jwt.InvalidTokenError:
            raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Invalid token")
