from fastapi import FastAPI
from app.core.logging import logging

logging.info("Starting FastAPI application...")
app = FastAPI()