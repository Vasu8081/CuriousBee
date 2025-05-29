from fastapi import FastAPI
from app.routes import auth, users, groups
from app.core.logging import logging

logging.info("Starting FastAPI application...")
app = FastAPI()

app.include_router(auth.router, prefix="/auth")
app.include_router(users.router, prefix="/users")
app.include_router(groups.router, prefix="/groups")