from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.orm import Session
from pydantic import EmailStr
from app.core.dependencies import get_current_user_email
from app.db.database import get_db
from app.db.models.groups import Groups
from app.db.models.users import Users
from app.db.schemas.period_entries_schema import PeriodEntriesSchema

router = APIRouter()

@router.get("/periods", response_model=list[PeriodEntriesSchema])
async def get_periods(
    db: Session = Depends(get_db),
    current_user_email: EmailStr = Depends(get_current_user_email)
):
    """
    Retrieve all periods for the current user.
    """
    user = db.query(Users).filter(Users._email == current_user_email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")
    
    if not user.group:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User group not found")

    return [entry.to_schema() for entry in user.group.period_entries]