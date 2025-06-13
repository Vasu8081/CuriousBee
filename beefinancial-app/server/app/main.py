from fastapi import FastAPI
from app.core.logging import logging
from app.routes import (
auth, users
)

logging.info("Starting FastAPI application...")
app = FastAPI()

app.include_router(auth.router, prefix="/auth")
app.include_router(users.router, prefix="/users")