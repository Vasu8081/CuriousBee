from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.orm import Session
from pydantic import BaseModel, EmailStr
from app.core.dependencies import get_current_user_email
from app.db.database import get_db
from app.db.models.groups import Groups
from app.db.models.users import Users

router = APIRouter()
