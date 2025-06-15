from fastapi import FastAPI
from app.core.logging import logging
from app.routes import (
auth, users, accounts, transactions
)

logging.info("Starting FastAPI application...")
app = FastAPI()

app.include_router(auth.router, prefix="/auth")
app.include_router(users.router, prefix="/users")
app.include_router(accounts.router, prefix="/accounts")
app.include_router(transactions.router, prefix="/transactions")