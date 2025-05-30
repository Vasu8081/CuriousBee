from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.orm import Session
from pydantic import EmailStr
from app.core.dependencies import get_current_user_email
from app.db.database import get_db
from app.db.autogen.models.groups import Groups
from app.db.autogen.models.users import Users
from app.db.autogen.schemas.period_entries_schema import PeriodEntriesSchema
from app.db.autogen.schemas.groups_schema import GroupsSchema
import uuid

router = APIRouter()

@router.get("/{group_id}")
def get_group_period_entries(
    group_id: str,
    email: str = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    from app.db.autogen.models.users import Users
    from app.db.autogen.models.groups import Groups
    from app.db.autogen.models.calendar_entries import CalendarEntries
    from app.db.autogen.schemas.calendar_entries_schema import CalendarEntriesSchema

    user = db.query(Users).filter(Users._email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    users = db.query(Users).filter(Users._group_id == group_id).all()

    cal_entries = []
    for user in users:
        entries = db.query(CalendarEntries).filter(CalendarEntries._user_id == user._id).all()
        cal_entries.extend(entries)

    return [cal_entrie.to_schema() for cal_entrie in cal_entries]