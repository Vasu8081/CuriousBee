# app/db/database.py
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker, Session
import os
from app.core.logging import logging

DATABASE_URL = os.getenv("DATABASE_URL", "postgresql://vkandula:vkandula@localhost/beetracker")
logging.info(f"Using database URL: {DATABASE_URL}")
engine = create_engine(DATABASE_URL)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)

def get_db() -> Session:
    """Returns a new SQLAlchemy session."""
    return SessionLocal()