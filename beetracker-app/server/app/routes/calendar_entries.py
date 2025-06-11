from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.orm import Session
from pydantic import EmailStr
from app.core.dependencies import get_current_user_email
from app.db.database import get_db
from app.autogen.schemas.calendar_entries_schema import CalendarEntriesSchema
import uuid

router = APIRouter()

@router.get("/{group_id}")
def get_group_period_entries(
    group_id: str,
    email: str = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    from app.autogen.models.users import Users
    from app.autogen.models.calendar_entries import CalendarEntries

    user = db.query(Users).filter(Users.__table__.c._email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    users = db.query(Users).filter(Users.__table__.c._group_id == group_id).all()

    cal_entries = []
    for user in users:
        entries = db.query(CalendarEntries).filter(CalendarEntries.__table__.c._user_id == user.Id).all()
        cal_entries.extend(entries)

    return [cal_entrie.to_schema() for cal_entrie in cal_entries]

@router.post("/{group_id}")
def create_group_period_entry(
    group_id: str,
    calendar_entries_schema: CalendarEntriesSchema,
    email: EmailStr = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    from app.autogen.models.users import Users
    from app.autogen.models.groups import Groups
    from app.autogen.models.calendar_entries import CalendarEntries

    user = db.query(Users).filter(Users.__table__.c._email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    group = db.query(Groups).filter(Groups.__table__.c._id == group_id).first()
    if not group:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Group not found")
    
    existing_entry = db.query(CalendarEntries).filter(CalendarEntries.__table__.c._id == calendar_entries_schema.id).first()

    if existing_entry:
        updated = calendar_entries_schema.to_model()
        updated._id = existing_entry.Id
        updated = db.merge(updated)  # 🔁 assign the returned instance
    else:
        new_entry = calendar_entries_schema.to_model()
        new_entry._id = calendar_entries_schema.id or uuid.uuid4()
        db.add(new_entry)

    db.commit()
    db.refresh(new_entry if not existing_entry else updated)

    return (new_entry if not existing_entry else updated).to_schema()


@router.delete("/{calendar_entry_id}")
def delete_group_period_entry(
    calendar_entry_id: str,
    email: EmailStr = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    from app.autogen.models.users import Users
    from app.autogen.models.groups import Groups
    from app.autogen.models.calendar_entries import CalendarEntries

    user = db.query(Users).filter(Users.__table__.c._email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    entry = db.query(CalendarEntries).filter(CalendarEntries.__table__.c._id == calendar_entry_id).first()
    if not entry:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Calendar entry not found")

    db.delete(entry)
    db.commit()

    return {"detail": "Calendar entry deleted successfully"}