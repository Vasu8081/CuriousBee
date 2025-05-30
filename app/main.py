from fastapi import FastAPI
from app.routes import auth, users, groups, calendar_entries
from app.core.logging import logging

logging.info("Starting FastAPI application...")
app = FastAPI()

app.include_router(auth.router, prefix="/auth")
app.include_router(users.router, prefix="/users")
app.include_router(groups.router, prefix="/groups")
app.include_router(calendar_entries.router, prefix="/calendar_entries")