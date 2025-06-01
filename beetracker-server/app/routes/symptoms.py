from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.orm import Session
from pydantic import EmailStr
from app.core.dependencies import get_current_user_email
from app.db.database import get_db
from app.core.logging import logging
from app.db.autogen.models.groups import Groups
from app.db.autogen.models.users import Users
from app.db.autogen.schemas.period_entries_schema import PeriodEntriesSchema
from app.db.autogen.schemas.groups_schema import GroupsSchema
import uuid

router = APIRouter()

@router.get("/{group_id}")
def get_group_period_entries(
    group_id: uuid.UUID,
    email: str = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    from app.db.autogen.models.symptoms import Symptoms

    user = db.query(Users).filter(Users._email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    if user._group_id != group_id:
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="You do not have permission to view this group's period entries")
    
    symptoms = db.query(Symptoms).all()

    return [symptom.to_schema() for symptom in symptoms]