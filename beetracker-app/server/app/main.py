from fastapi import FastAPI
from app.routes import auth, users, groups, calendar_entries, period_infos, symptoms, tasks, product_infos, product_status, period_entries
from app.core.logging import logging

logging.info("Starting FastAPI application...")
app = FastAPI()

app.include_router(auth.router, prefix="/auth")
app.include_router(users.router, prefix="/users")
app.include_router(groups.router, prefix="/groups")
app.include_router(calendar_entries.router, prefix="/calendar_entries")
app.include_router(period_infos.router, prefix="/period_infos")
app.include_router(symptoms.router, prefix="/symptoms")
app.include_router(tasks.router, prefix="/tasks")
app.include_router(product_infos.router, prefix="/product_infos")
app.include_router(product_status.router, prefix="/product_status")
app.include_router(period_entries.router, prefix="/period_entries")