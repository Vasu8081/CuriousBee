# app/db/session.py
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
import os
from app.core.logging import logging

DATABASE_URL = os.getenv("DATABASE_URL", "postgresql://vkandula:vkandula@localhost/beetracker")
logging.info(f"Using database URL: {DATABASE_URL}")
engine = create_engine(DATABASE_URL, echo=True)
SessionLocal = sessionmaker(bind=engine, autocommit=False, autoflush=False)